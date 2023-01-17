#include <device/e1000.h>
#include <device/ethernet.h>
#include <device/interrupt_register.h>
#include <device/pci.h>
#include <kernel/memman.h>
#include <kernel/pagepte.h>
#include <kernel/spinlock.h>
#include <kernlib/assert.h>
#include <kernlib/mbuf.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

uint32_t *e1000_regs;

struct spinlock e1000_lock;

static struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
static struct mbuf   *tx_mbufs[TX_RING_SIZE];

static struct rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));
static struct mbuf   *rx_mbufs[RX_RING_SIZE];

// called by pci_init().
// xregs is the memory address at which the
// e1000's registers are mapped.
void e1000_init()
{
    int i;

    initlock(&e1000_lock, "e1000");

    // Reset the device
    e1000_regs[E1000_IMS] = 0;  // disable interrupts
    e1000_regs[E1000_CTL] |= E1000_CTL_RST;
    e1000_regs[E1000_IMS] = 0;  // redisable interrupts
    __sync_synchronize();

    // [E1000 14.5] Transmit initialization
    memset(tx_ring, 0, sizeof(tx_ring));
    for (i = 0; i < TX_RING_SIZE; i++) {
        tx_ring[i].status = E1000_TXD_STAT_DD;
        tx_mbufs[i] = 0;
    }
    e1000_regs[E1000_TDBAL] = (uint32_t)tx_ring;
    if (sizeof(tx_ring) % 128 != 0)
        panic("e1000");
    e1000_regs[E1000_TDLEN] = sizeof(tx_ring);
    e1000_regs[E1000_TDH] = e1000_regs[E1000_TDT] = 0;

    // [E1000 14.4] Receive initialization
    memset(rx_ring, 0, sizeof(rx_ring));
    for (i = 0; i < RX_RING_SIZE; i++) {
        rx_mbufs[i] = mbufalloc(0);
        if (!rx_mbufs[i])
            panic("e1000");
        rx_ring[i].addr = (uint32_t)rx_mbufs[i]->head;
    }
    e1000_regs[E1000_RDBAL] = (uint32_t)rx_ring;
    if (sizeof(rx_ring) % 128 != 0)
        panic("e1000");
    e1000_regs[E1000_RDH] = 0;
    e1000_regs[E1000_RDT] = RX_RING_SIZE - 1;
    e1000_regs[E1000_RDLEN] = sizeof(rx_ring);

    // TODO: 尽量不要使用硬编码
    // filter by qemu's MAC address, 52:54:00:12:34:56
    e1000_regs[E1000_RA] = 0x12005452;
    e1000_regs[E1000_RA + 1] = 0x5634 | (1 << 31);
    // multicast table
    for (int i = 0; i < 4096 / 32; i++)
        e1000_regs[E1000_MTA + i] = 0;

    // transmitter control bits.
    e1000_regs[E1000_TCTL] = E1000_TCTL_EN |                  // enable
                       E1000_TCTL_PSP |                 // pad short packets
                       (0x10 << E1000_TCTL_CT_SHIFT) |  // collision stuff
                       (0x40 << E1000_TCTL_COLD_SHIFT);
    e1000_regs[E1000_TIPG] = 10 | (8 << 10) | (6 << 20);  // inter-pkt gap

    // receiver control bits.
    e1000_regs[E1000_RCTL] = E1000_RCTL_EN |       // enable receiver
                       E1000_RCTL_BAM |      // enable broadcast
                       E1000_RCTL_SZ_2048 |  // 2048-byte rx buffers
                       E1000_RCTL_SECRC;     // strip CRC

    // ask e1000 for receive interrupts.
    e1000_regs[E1000_RDTR] = 0;        // interrupt after every received packet (no timer)
    e1000_regs[E1000_RADV] = 0;        // interrupt after every packet (no timer)
    e1000_regs[E1000_IMS] = (1 << 7);  // RXDW -- Receiver Descriptor Write Back

    kprintf("e1000 init succeed\n");
}

int e1000_transmit(struct mbuf *m)
{
    // the mbuf contains an ethernet frame; program it into
    // the TX descriptor ring so that the e1000 sends it. Stash
    // a pointer so that it can be freed after sending.
    //

    // First ask the E1000 for the TX ring index at which it's expecting the next packet,
    // by reading the E1000_TDT control register.
    acquire(&e1000_lock);

    uint32_t index = e1000_regs[E1000_TDT];

    // check if the the ring is overflowing.
    // If E1000_TXD_STAT_DD is not set in the descriptor indexed by E1000_TDT,
    // the E1000 hasn't finished the corresponding previous transmission request,
    // so return an error.

    if ((tx_ring[index].status & E1000_TXD_STAT_DD) == 0) {
        release(&e1000_lock);
        kprintf("buffer overflow\n");
        return -1;
    }


    // use mbuffree() to free the last mbuf that
    // was transmitted from that descriptor (if there was one).
    if (tx_mbufs[index])
        mbuffree(tx_mbufs[index]);

    tx_ring[index].addr = (uint32_t)m->head;
    tx_ring[index].length = m->len;
    tx_ring[index].cmd = E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP;
    tx_mbufs[index] = m;


    e1000_regs[E1000_TDT] = (index + 1) % TX_RING_SIZE;
    __sync_synchronize();
    release(&e1000_lock);

    return 0;
}

void e1000_recv(void)
{
    // Check for packets that have arrived from the e1000
    // Create and deliver an mbuf for each packet (using net_rx()).
    //
    for (;;) {
        // next index
        uint32_t index = (e1000_regs[E1000_RDT] + 1) % RX_RING_SIZE;

        // check status, if not set we will return
        if ((rx_ring[index].status & E1000_RXD_STAT_DD) == 0)
            break;

        // deliver to network stack
        rx_mbufs[index]->len = rx_ring[index].length;
        rx_mbufs[index]->head = rx_mbufs[index]->buf;
        eth_rx(rx_mbufs[index]);

        // allocate a new mbuf using mbufalloc()
        // to replace the one just given to net_rx().
        // Program its data pointer (m->head) into the descriptor.
        // Clear the descriptor's status bits to zero.
        rx_mbufs[index] = mbufalloc(0);
        rx_ring[index].addr = (uint32_t)rx_mbufs[index]->head;
        rx_ring[index].status = 0;

        // current index
        e1000_regs[E1000_RDT] = index;
    }
}

int pci_e1000_attach(struct pci_func *pcif)
{
    kprintf("ready to start e1000...\n");

    // 使能改设备，并读取 bar 的相关信息
    pci_func_enable(pcif);

    // 由于 e1000 只用到第一个 bar（第二个是 io bar），这一段可以根据设备硬编码
    // 直接 mmio_map 到虚拟内存里就好
    // 这一步之后，就不用走 inl 和 outl 了，直接读写内存即可
    e1000_regs = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);

    // 注册一下 receive 的中断
    // TODO: 改为 MSI 中断
    register_device_interrupt(pcif->irq_line, DA_386IGate, e1000_receive_pack_handler, PRIVILEGE_KRNL);

    // E1000 初始化
    e1000_init(regs);
    kprintf("device status: %08x\n", regs[E1000_DEVICE_STATUS]);

    while(1) {}

    // e1000_transmit_init();
    // e1000_receive_init();
    
    return 0;
}

// 接收中断处理函数
// 将网卡的 pack 保存到数组里
void e1000_receive_pack_handler()
{
    // use int INT_VECTOR_E1000_RECEIVE_PACK
    // and put your data in pack

    __asm__ __volatile__(
        "pushfl\n\tpushal\n\t"
    );

    kprintf("begin to receive e1000's pack now...\n");

    __asm__ __volatile__(
        "popal\n\tpopfl\n\t"
    );
}
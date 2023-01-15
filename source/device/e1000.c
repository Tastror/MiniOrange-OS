#include <device/mbuf.h>
#include <device/e1000.h>
#include <device/interrupt_register.h>
#include <device/pci.h>
#include <kernlib/stdio.h>
#include <kernlib/assert.h>
#include <kernlib/string.h>
#include <kernel/spinlock.h>

volatile uint32_t *e1000;

struct spinlock e1000_lock;

uint32_t regs[64]; // size is 32/64

static struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
static struct mbuf *tx_mbufs[TX_RING_SIZE];

static struct rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));
static struct mbuf *rx_mbufs[RX_RING_SIZE];

// called by pci_init().
// xregs is the memory address at which the
// e1000's registers are mapped.
void
e1000_init(uint32_t *regs)
{
    int i;
    
    initlock(&e1000_lock, "e1000");

    // Reset the device
    regs[E1000_IMS] = 0; // disable interrupts
    regs[E1000_CTL] |= E1000_CTL_RST;
    regs[E1000_IMS] = 0; // redisable interrupts
    __sync_synchronize();

    // [E1000 14.5] Transmit initialization
    memset(tx_ring, 0, sizeof(tx_ring));
    for (i = 0; i < TX_RING_SIZE; i++) {
        tx_ring[i].status = E1000_TXD_STAT_DD;
        tx_mbufs[i] = 0;
    }
    regs[E1000_TDBAL] = (uint32_t) tx_ring;
    if(sizeof(tx_ring) % 128 != 0)
        panic("e1000");
    regs[E1000_TDLEN] = sizeof(tx_ring);
    regs[E1000_TDH] = regs[E1000_TDT] = 0;
    
    // [E1000 14.4] Receive initialization
    memset(rx_ring, 0, sizeof(rx_ring));
    for (i = 0; i < RX_RING_SIZE; i++) {
        rx_mbufs[i] = mbufalloc(0);
        if (!rx_mbufs[i])
        panic("e1000");
        rx_ring[i].addr = (uint32_t) rx_mbufs[i]->head;
    }
    regs[E1000_RDBAL] = (uint32_t) rx_ring;
    if(sizeof(rx_ring) % 128 != 0)
        panic("e1000");
    regs[E1000_RDH] = 0;
    regs[E1000_RDT] = RX_RING_SIZE - 1;
    regs[E1000_RDLEN] = sizeof(rx_ring);

    // TODO: 尽量不要使用硬编码
    // filter by qemu's MAC address, 52:54:00:12:34:56
    regs[E1000_RA] = 0x12005452;
    regs[E1000_RA+1] = 0x5634 | (1<<31);
    // multicast table
    for (int i = 0; i < 4096/32; i++)
        regs[E1000_MTA + i] = 0;

    // transmitter control bits.
    regs[E1000_TCTL] = E1000_TCTL_EN |  // enable
        E1000_TCTL_PSP |                  // pad short packets
        (0x10 << E1000_TCTL_CT_SHIFT) |   // collision stuff
        (0x40 << E1000_TCTL_COLD_SHIFT);
    regs[E1000_TIPG] = 10 | (8<<10) | (6<<20); // inter-pkt gap

    // receiver control bits.
    regs[E1000_RCTL] = E1000_RCTL_EN | // enable receiver
        E1000_RCTL_BAM |                 // enable broadcast
        E1000_RCTL_SZ_2048 |             // 2048-byte rx buffers
        E1000_RCTL_SECRC;                // strip CRC              
  
    // ask e1000 for receive interrupts.
    regs[E1000_RDTR] = 0; // interrupt after every received packet (no timer)
    regs[E1000_RADV] = 0; // interrupt after every packet (no timer)
    regs[E1000_IMS] = (1 << 7); // RXDW -- Receiver Descriptor Write Back

    kprintf("e1000 init succeed\n");
}

int pci_e1000_attach(struct pci_func *pcif)
{
    kprintf("ready to start e1000...\n");
    // 写 E1000 寄存器，使能 E1000
    pci_func_enable(pcif);
    // 注册一下 receive 的中断
    register_device_interrupt(pcif->irq_line, DA_386IGate, e1000_receive_pack_handler, PRIVILEGE_KRNL);
    // E1000 初始化
    e1000_init(regs);
    // e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    // e1000_transmit_init();
    // e1000_receive_init();
    // cprintf("device status:[%08x]\n", e1000[E1000_LOCATE(E1000_DEVICE_STATUS)]);
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
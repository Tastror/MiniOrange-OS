#include <device/e1000.h>

#include <device/interrupt_register.h>
#include <device/pci.h>
#include <kernlib/stdio.h>

volatile uint32_t *e1000;

int pci_e1000_attach(struct pci_func *pcif)
{
    kprintf("ready to start e1000...\n");
    // 使能E1000，分配 MMIO等
    pci_func_enable(pcif);
    // 注册一下 receive 的中断
    register_device_interrupt(pcif->irq_line, DA_386IGate, e1000_receive_pack_handler, PRIVILEGE_KRNL);
    // 映射，并保存其虚拟地址，方便访问。
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
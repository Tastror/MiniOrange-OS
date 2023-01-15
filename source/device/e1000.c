#include <device/e1000.h>

#include <device/pci.h>
#include <kernlib/stdio.h>

volatile uint32_t *e1000;

int pci_e1000_attach(struct pci_func *pcif)
{
    kern_set_color(CYAN);
    kprintf("ready to start e1000...\n");
    kern_set_color(WHITE);
    int r;
    // 使能E1000，分配 MMIO等
    pci_func_enable(pcif);
    // 映射，并保存其虚拟地址，方便访问。
    // e1000 = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    // e1000_transmit_init();
    // e1000_receive_init();
    // cprintf("device status:[%08x]\n", e1000[E1000_LOCATE(E1000_DEVICE_STATUS)]);
    return 0;
}
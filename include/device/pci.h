#ifndef _ORANGES_PCI_H
#define _ORANGES_PCI_H

#include <common/type.h>
#include <define/define.h>

// PCI subsystem interface
enum {
    pci_res_bus,
    pci_res_mem,
    pci_res_io,
    pci_res_max
};

struct pci_func {
<<<<<<< HEAD
    struct pci_bus *bus;  // Primary bus for bridges

    uint32_t dev;   // device
    uint32_t func;  // function

    uint32_t dev_id;     // read from device by PCI_ID_REG
    uint32_t dev_class;  // read from device by PCI_CLASS_REG

    // 6 bars
    uint32_t reg_base[6];
    uint32_t reg_size[6];
    enum {PCI_NOT_MEMORY_BAR = 0, PCI_MEMORY_BAR = 1} reg_type[6];

    uint8_t irq_line;  // read from device by PCI_INTERRUPT_REG
=======
    struct pci_bus *bus;          // Primary bus for bridges
    uint32_t        dev;          // device
    uint32_t        func;         // function
    uint32_t        dev_id;       // read from device by PCI_ID_REG
    uint32_t        dev_class;    // read from device by PCI_CLASS_REG
    uint32_t        reg_base[6];  // bar base
    uint32_t        reg_size[6];  // bar size
    uint8_t         irq_line;     // read from device by PCI_INTERRUPT_REG
>>>>>>> b9610aed26056436767a3a73ad0cf2b9bce2e430
};

struct pci_bus {
    struct pci_func *parent_bridge;
    uint32_t         busno;
};

int  init_pci_msi(void);
int  init_pci_device(void);
void pci_func_enable(struct pci_func *f);

#endif
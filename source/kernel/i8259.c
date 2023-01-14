
/**
 * @file i8259.c
 * @author Forrest Yu
 * @date 2005
 */

#include "common/type.h"
#include "device/x86.h"
#include "kernel/global.h"
#include "kernel/proto.h"
#include "lib/stdio.h"
#include "software_define/kern_const.h"
#include "software_define/proc_define.h"
#include "software_define/protect_define.h"


/**
 * init_8259A
 */
void init_8259A()
{
    outb(INT_M_CTL, 0x11);                 // Master 8259, ICW1.
    outb(INT_S_CTL, 0x11);                 // Slave  8259, ICW1.
    outb(INT_M_CTLMASK, INT_VECTOR_IRQ0);  // Master 8259, ICW2. 设置 '主8259' 的中断入口地址为 0x20.
    outb(INT_S_CTLMASK, INT_VECTOR_IRQ8);  // Slave  8259, ICW2. 设置 '从8259' 的中断入口地址为 0x28
    outb(INT_M_CTLMASK, 0x4);              // Master 8259, ICW3. IR2 对应 '从8259'.
    outb(INT_S_CTLMASK, 0x2);              // Slave  8259, ICW3. 对应 '主8259' 的 IR2.
    outb(INT_M_CTLMASK, 0x1);              // Master 8259, ICW4.
    outb(INT_S_CTLMASK, 0x1);              // Slave  8259, ICW4.

    outb(INT_M_CTLMASK, 0xFF);  // Master 8259, OCW1.
    outb(INT_S_CTLMASK, 0xFF);  // Slave  8259, OCW1.

    int i;
    for (i = 0; i < NR_IRQ; i++) {
        irq_table[i] = spurious_irq;
    }
}

void disable_irq(int irq)
{
    u8 mask = 1 << (irq % 8);
    if (irq < 8)
        outb(INT_M_CTLMASK, inb(INT_M_CTLMASK) | mask);
    else
        outb(INT_S_CTLMASK, inb(INT_S_CTLMASK) | mask);
}

void enable_irq(int irq)
{
    u8 mask = 1 << (irq % 8);
    if (irq < 8)
        outb(INT_M_CTLMASK, inb(INT_M_CTLMASK) & ~mask);
    else
        outb(INT_S_CTLMASK, inb(INT_S_CTLMASK) & ~mask);
}

/*======================================================================*
                           spurious_irq
 *======================================================================*/
void spurious_irq(int irq)
{
    kprintf("spurious_irq: %d\n", irq);
}

/*======================================================================*
                           put_irq_handler
 *======================================================================*/
void put_irq_handler(int irq, irq_handler handler)
{
    disable_irq(irq);
    irq_table[irq] = handler;
}

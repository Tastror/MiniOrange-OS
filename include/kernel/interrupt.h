#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <common/type.h>
#include <define/define.h>

extern irq_handler irq_table[NR_IRQ];  // in i8259.c

void put_irq_handler(int irq, irq_handler handler);
void spurious_irq(int irq);
void disable_irq(int irq);
void enable_irq(int irq);
void init_8259A();

void clock_handler(int irq);

#endif
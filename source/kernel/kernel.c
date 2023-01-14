#include <kernel/kernel.h>

int        kernel_initial;
u32        k_reenter;
PROCESS    cpu_table[NR_CPUS];
DESCRIPTOR gdt[GDT_SIZE];
GATE       idt[IDT_SIZE];
u8         gdt_ptr[6];
u8         idt_ptr[6];
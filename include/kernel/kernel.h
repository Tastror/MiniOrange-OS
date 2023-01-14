#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <common/type.h>
#include <define/define.h>

// in kernel.c
extern int        kernel_initial;
extern u32        k_reenter;
extern PROCESS    cpu_table[NR_CPUS];
extern DESCRIPTOR gdt[GDT_SIZE];
extern GATE       idt[IDT_SIZE];
extern u8         gdt_ptr[6];
extern u8         idt_ptr[6];

u32 get_arg(void *uesp, int order);  // added by xw, 18/6/18

// void restart_int();
// void save_context();
void restart_initial();  // added by xw, 18/4/18
void restart_restore();  // added by xw, 18/4/20
void halt();             // added by xw, 18/6/11

/* ktest.c */
void initial();

#endif
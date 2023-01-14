#ifndef _PROC_H_
#define _PROC_H_

#include <common/type.h>
#include <define/define.h>

#include <kernel/hd.h>
#include <kernel/tty.h>

// in proc.c
extern PROCESS *p_proc_current;
extern PROCESS *p_proc_next;
extern PROCESS proc_table[NR_PCBS];
extern u32 u_proc_sum;
extern TASK task_table[NR_TASKS];

#define proc2pid(x) (x - proc_table)

PROCESS *alloc_PCB();
void     free_PCB(PROCESS *p);
int      ldt_seg_linear(PROCESS *p, int idx);
void    *va2la(int pid, void *va);
void     sched();

#endif
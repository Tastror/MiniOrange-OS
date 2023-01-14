#ifndef _PROTECT_H_
#define _PROTECT_H_

#include <common/type.h>
#include <define/define.h>

extern TSS tss; // in protect.c

void init_prot();
u32  seg2phys(u16 seg);

#endif
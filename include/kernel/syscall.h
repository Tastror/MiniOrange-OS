#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <common/type.h>
#include <define/define.h>

extern int ticks;  // in clock.c

// syscall system interface
int   sys_get_ticks();
int   sys_get_pid();                // add by visual 2016.4.6
void *sys_kmalloc(int size);        // edit by visual 2016.5.9
void *sys_kmalloc_4k();             // edit by visual 2016.5.9
void *sys_malloc(int size);         // edit by visual 2016.5.9
void *sys_malloc_4k();              // edit by visual 2016.5.9
int   sys_free(void *arg);          // edit by visual 2016.5.9
int   sys_free_4k(void *AdddrLin);  // edit by visual 2016.5.9
int   sys_pthread(void *arg);       // add by visual 2016.4.11
void  sys_display_int(int arg);     // add by visual 2016.5.16
void  sys_display_str(char *arg);   // add by visual 2016.5.16
void  sys_set_color(int color);     // add by tastror 2020.12
void  sys_yield();
void  sys_sleep(int n);
void  sys_wakeup(void *channel);
u32   sys_exec(char *path);  // exec.c, add by visual 2016.5.23
int   sys_fork();            // fork.c, add by visual 2016.5.25
void  sys_print_E();         // testfunc.c
void  sys_print_F();         // testfunc.c
void  sys_net_test(u8 *args);

#endif
#ifndef __USER_STDLIB_H__
#define __USER_STDLIB_H__

#pragma once

#include <common/type.h>
#include <define/define.h>

void  delay(int time);
int   get_ticks();
int   get_pid();
void *kmalloc(int size);
void *kmalloc_4k();
void *malloc(int size);
void *malloc_4k();
int   free(void *arg);
int   free_4k(void *AdddrLin);
int   fork();
int   pthread(void *arg);
u32   exec(char *path);
void  yield();
void  sleep(int n);
void  print_E();
void  print_F();
void  net_test(u8 *args);
void  net_test_shell(u16 len, u8 *data);

#endif
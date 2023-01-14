#ifndef _OS_SYSCALL_USER_H_
#define _OS_SYSCALL_USER_H_

#include <common/type.h>
#include <define/define.h>

ssize_t syscall_0(size_t NR_syscall);
ssize_t syscall_1(size_t NR_syscall, size_t p1);
ssize_t syscall_esp(size_t NR_syscall, size_t any, ...);

#endif
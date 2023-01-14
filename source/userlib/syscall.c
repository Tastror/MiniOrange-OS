#include <userlib/syscall.h>

ssize_t syscall_0(size_t NR_syscall)
{
    ssize_t ret;
    __asm__ __volatile__(
        "int %1"
        : "=a"(ret)
        : "i"(INT_VECTOR_SYS_CALL), "a"(NR_syscall)
        : "cc", "memory"
    );
    return ret;
}

ssize_t syscall_1(size_t NR_syscall, size_t p1)
{
    ssize_t ret;
    __asm__ __volatile__(
        "int %1"
        : "=a"(ret)
        : "i"(INT_VECTOR_SYS_CALL), "a"(NR_syscall), "b"(p1)
        : "cc", "memory"
    );
    return ret;
}

ssize_t syscall_esp(size_t NR_syscall, size_t any, ...)
{
    ssize_t ret;
    __asm__ __volatile__(
        "mov %%ebx, %%esp\n\t"
        "add %%ebx, 8\n\t"
        "int %1"
        : "=a"(ret)
        : "i"(INT_VECTOR_SYS_CALL), "a"(NR_syscall)
        : "ebx", "cc", "memory"
    );
    return ret;
}
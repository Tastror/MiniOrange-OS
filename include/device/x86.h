#ifndef _MINIOS_X86_H_
#define _MINIOS_X86_H_

#include <common/type.h>
#include <define/define.h>

static inline u8 inb(int port)
{
    u8 data;
    __asm__ __volatile__("inb %w1,%0"
                         : "=a"(data)
                         : "d"(port));
    return data;
}

static inline void insb(int port, void *addr, int cnt)
{
    __asm__ __volatile__(
        "cld\n\trepne\n\tinsb"
        : "=D"(addr), "=c"(cnt)
        : "d"(port), "0"(addr), "1"(cnt)
        : "memory", "cc"
    );
}

static inline u16 inw(int port)
{
    u16 data;
    __asm__ __volatile__("inw %w1,%0"
                         : "=a"(data)
                         : "d"(port));
    return data;
}

static inline void insw(int port, void *addr, int cnt)
{
    __asm__ __volatile__(
        "cld\n\trepne\n\tinsw"
        : "=D"(addr), "=c"(cnt)
        : "d"(port), "0"(addr), "1"(cnt)
        : "memory", "cc"
    );
}

static inline u32 inl(int port)
{
    u32 data;
    __asm__ __volatile__("inl %w1,%0"
                         : "=a"(data)
                         : "d"(port));
    return data;
}

static inline void insl(int port, void *addr, int cnt)
{
    __asm__ __volatile__(
        "cld\n\trepne\n\tinsl"
        : "=D"(addr), "=c"(cnt)
        : "d"(port), "0"(addr), "1"(cnt)
        : "memory", "cc"
    );
}

static inline void outb(int port, u8 data)
{
    __asm__ __volatile__("outb %0,%w1"
                         :
                         : "a"(data), "d"(port));
}

static inline void outsb(int port, const void *addr, int cnt)
{
    __asm__ __volatile__(
        "cld\n\trepne\n\toutsb"
        : "=S"(addr), "=c"(cnt)
        : "d"(port), "0"(addr), "1"(cnt)
        : "cc"
    );
}

static inline void outw(int port, u16 data)
{
    __asm__ __volatile__("outw %0,%w1"
                         :
                         : "a"(data), "d"(port));
}

static inline void outsw(int port, const void *addr, int cnt)
{
    __asm__ __volatile__(
        "cld\n\trepne\n\toutsw"
        : "=S"(addr), "=c"(cnt)
        : "d"(port), "0"(addr), "1"(cnt)
        : "cc"
    );
}

static inline void outsl(int port, const void *addr, int cnt)
{
    __asm__ __volatile__(
        "cld\n\trepne\n\toutsl"
        : "=S"(addr), "=c"(cnt)
        : "d"(port), "0"(addr), "1"(cnt)
        : "cc"
    );
}

static inline void outl(int port, u32 data)
{
    __asm__ __volatile__("outl %0,%w1"
                         :
                         : "a"(data), "d"(port));
}

static inline void lcr0(u32 val)
{
    __asm__ __volatile__("movl %0,%%cr0"
                         :
                         : "r"(val));
}

static inline u32 rcr0(void)
{
    u32 val;
    __asm__ __volatile__("movl %%cr0,%0"
                         : "=r"(val));
    return val;
}

static inline u32 rcr2(void)
{
    u32 val;
    __asm__ __volatile__("movl %%cr2,%0"
                         : "=r"(val));
    return val;
}

static inline void lcr3(u32 val)
{
    __asm__ __volatile__("movl %0,%%cr3"
                         :
                         : "r"(val));
}

static inline u32 rcr3(void)
{
    u32 val;
    __asm__ __volatile__("movl %%cr3,%0"
                         : "=r"(val));
    return val;
}

static inline void lcr4(u32 val)
{
    __asm__ __volatile__("movl %0,%%cr4"
                         :
                         : "r"(val));
}

static inline u32 rcr4(void)
{
    u32 cr4;
    __asm__ __volatile__("movl %%cr4,%0"
                         : "=r"(cr4));
    return cr4;
}

static inline void tlbflush(void)
{
    u32 cr3;
    __asm__ __volatile__("movl %%cr3,%0"
                         : "=r"(cr3));
    __asm__ __volatile__("movl %0,%%cr3"
                         :
                         : "r"(cr3));
}

static inline u32 read_eflags(void)
{
    u32 eflags;
    __asm__ __volatile__("pushfl\n\t popl %0"
                         : "=r"(eflags));
    return eflags;
}

static inline void write_eflags(u32 eflags)
{
    __asm__ __volatile__("pushl %0\n\t popfl"
                         :
                         : "r"(eflags));
}

static inline u32 read_ebp(void)
{
    u32 ebp;
    __asm__ __volatile__("movl %%ebp,%0"
                         : "=r"(ebp));
    return ebp;
}

static inline u32 read_esp(void)
{
    u32 esp;
    __asm__ __volatile__("movl %%esp,%0"
                         : "=r"(esp));
    return esp;
}

static inline u64 read_tsc(void)
{
    u64 tsc;
    __asm__ __volatile__("rdtsc"
                         : "=A"(tsc));
    return tsc;
}

static inline u32 xchg(__volatile__ u32 *addr, u32 newval)
{
    u32 result;

    // The + in "+m" denotes a read-modify-write operand.
    __asm__ __volatile__(
        "lock\n\t xchgl %0, %1"
        : "+m"(*addr), "=a"(result)
        : "1"(newval)
        : "cc"
    );
    return result;
}

static inline void disable_int()
{
    __asm__ __volatile__("cli");
}

static inline void enable_int()
{
    __asm__ __volatile__("sti");
}

// defined in kernel.c
extern int int_disable_time;

static inline void nested_disable_int()
{
    int_disable_time++;
    if (int_disable_time > 0)
        __asm__ __volatile__("cli");
}

static inline void nested_enable_int()
{
    int_disable_time--;
    if (int_disable_time <= 0)
        __asm__ __volatile__("sti");
}

static inline uint32_t log2(const uint32_t x)
{
    uint32_t y;
    __asm__ __volatile__(
        "bsr %1, %0\n\t"
        : "=r"(y)
        : "r"(x)
    );
    return y;
}

#endif /* _MINIOS_X86_H_ */
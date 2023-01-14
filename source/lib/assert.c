#include <lib/assert.h>

#include <common/stdarg.h>
#include <lib/stdio.h>

/*
 * 当发生不可挽回的错误时就打印错误信息并使CPU核休眠
 */
void _panic(const char *file, int line, const char *fmt, ...)
{
    va_list ap;

    // 确保CPU核不受外界中断的影响
    asm volatile("cli");
    asm volatile("cld");

    va_start(ap, fmt);
    kprintf("kernel panic at %s:%d: ", file, line);
    vkprintf(fmt, ap);
    kprintf("\n");
    va_end(ap);
    // 休眠CPU核，直接罢工
    while (true)
        __asm__ __volatile__("hlt");
}

/*
 * 很像panic，但是不会休眠CPU核，就是正常打印信息
 */
void _warn(const char *file, int line, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    kprintf("kernel warning at %s:%d: ", file, line);
    vkprintf(fmt, ap);
    kprintf("\n");
    va_end(ap);
}
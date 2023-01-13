#include "software_define/proc_define.h"
#include "software_define/protect_define.h"
#include "lib/klib.h"
#include "lib/stdio.h"

static void
kprintfputch(int ch, void *b)
{
    char buf[2] = {(char)ch, '\0'};
    kern_display_string(buf);
}

int vkprintf(const char *fmt, va_list ap)
{
    vprintfmt((void *)kprintfputch, NULL, fmt, ap);
    return 0;
}

int kprintf(const char *fmt, ...)
{
    va_list ap;
    int     rc;

    va_start(ap, fmt);
    rc = vkprintf(fmt, ap);
    va_end(ap);

    return rc;
}
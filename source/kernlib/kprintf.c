#include <kernlib/stdio.h>

static void
kprintfputch(int ch, void *b)
{
    kern_display_char(ch);
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
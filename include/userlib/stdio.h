#ifndef __USER_STDIO_H__
#define __USER_STDIO_H__

#include <common/stdarg.h>
#include <common/type.h>
#include <define/define.h>

int   vprintf(const char *fmt, va_list ap);
int   printf(const char *fmt, ...);
char  getchar();
char *gets(char *str);

void display_int(int arg);
void display_str(char *arg);
void set_color(int c);

#endif
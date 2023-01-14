/**
 * @file stdio.h
 * @brief 用户库函数声明
 * @author visual
 * @date 2016.5.16
 */

#ifndef _STDIO_H_
#define _STDIO_H_  // added by mingxuan 2019-5-19

#include <common/stdarg.h>
#include <common/type.h>
#include <define/define.h>

extern int disp_pos;
extern int saved_color;

#define isspace(s) (s == ' ')

#define TOLOWER(x)  ((x) | 0x20)
#define isxdigit(c) (('0' <= (c) && (c) <= '9') || ('a' <= (c) && (c) <= 'f') || ('A' <= (c) && (c) <= 'F'))
#define isdigit(c)  ('0' <= (c) && (c) <= '9')

void printfmt(void (*putch)(int, void *), void *putdat, const char *fmt, ...);
void vprintfmt(void (*putch)(int, void *), void *putdat, const char *fmt, va_list);
int  vsnprintf(char *buf, int n, const char *fmt, va_list ap);
int  snprintf(char *buf, int n, const char *fmt, ...);
int  vkprintf(const char *fmt, va_list ap);
int  kprintf(const char *fmt, ...);
void kern_set_color(int color);
void kern_display_string(char *info);
void kern_display_integer(int input);
void kern_display_char(char ch);

#endif  // added by mingxuan 2019-5-19
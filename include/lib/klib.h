#ifndef _KLIB_H_
#define _KLIB_H_

#include <common/type.h>
#include <software_define/define.h>
// #include <hardware_define/define.h>

char *itoa(char *str, int num);
void  delay(int time);
void  kern_set_color(int color);
void  kern_display_string(char *info);
void  kern_display_integer(int input);
void  kern_display_char(char ch);

#endif
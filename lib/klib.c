
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            klib.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "const.h"
#include "global.h"
#include "proc.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "type.h"


/*======================================================================*
                               itoa
 *======================================================================*/
char *itoa(char *str, int num) /* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
{
    char *p = str;
    char  ch;
    int   i;
    int   flag = FALSE;

    *p++ = '0';
    *p++ = 'x';

    if (num == 0) {
        *p++ = '0';
    } else {
        for (i = 28; i >= 0; i -= 4) {
            ch = (num >> i) & 0xF;
            if (flag || (ch > 0)) {
                flag = TRUE;
                ch += '0';
                if (ch > '9') {
                    ch += 7;
                }
                *p++ = ch;
            }
        }
    }

    *p = 0;

    return str;
}


/*======================================================================*
                               kern_display_integer
 *======================================================================*/
void kern_display_integer(int input)
{
    char output[16];
    itoa(output, input);
    kern_display_string(output);
}

/*======================================================================*
                               delay
 *======================================================================*/
void delay(int time)
{
    int i, j, k;
    for (k = 0; k < time; k++) {
        /*for(i=0;i<10000;i++){	for Virtual PC	*/
        for (i = 0; i < 10; i++) { /*	for Bochs	*/
            for (j = 0; j < 10000; j++) {}
        }
    }
}

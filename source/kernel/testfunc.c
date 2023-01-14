/**
 * testfunc.c
 * This file contains functions that are used to test features we add.
 * Added by xw, 18/6/17
 */

#include <lib/string.h>
#include <lib/stdio.h>

/*
 * This syscall needs long time to finish, so we can use it
 * to check if our os is kernel-preemptive.
 * added by xw, 18/4/27
 */
void sys_print_E()
{
    int i, j;

    kern_display_string("E( ");

    i = 100;
    while (--i) {
        j = 1000;
        while (--j) {}
    }

    kern_display_string(") ");
}

/*
 * This syscall needs long time to finish, so we can use it
 * to check if our os is kernel-preemptive.
 * added by xw, 18/4/27
 */
void sys_print_F()
{
    int i, j;

    kern_display_string("F( ");

    i = 100;
    while (--i) {
        j = 1000;
        while (--j) {}
    }

    kern_display_string(") ");
}

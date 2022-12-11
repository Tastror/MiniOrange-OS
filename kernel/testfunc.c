/**
 * testfunc.c
 * This file contains functions that are used to test features we add.
 * Added by xw, 18/6/17
 */

#include "const.h"
#include "global.h"
#include "proc.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "type.h"

/*
 * This syscall needs long time to finish, so we can use it
 * to check if our os is kernel-preemptive.
 * added by xw, 18/4/27
 */
void sys_print_E()
{
    int i, j;

    disp_str("E( ");

    i = 100;
    while (--i) {
        j = 1000;
        while (--j) {}
    }

    disp_str(") ");
}

/*
 * This syscall needs long time to finish, so we can use it
 * to check if our os is kernel-preemptive.
 * added by xw, 18/4/27
 */
void sys_print_F()
{
    int i, j;

    disp_str("F( ");

    i = 100;
    while (--i) {
        j = 1000;
        while (--j) {}
    }

    disp_str(") ");
}

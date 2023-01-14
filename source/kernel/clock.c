/**
 * @file clock.c
 * @author Forrest Yu
 * @date 2005
 */

#include <kernel/syscall.h>

#include <kernel/kernel.h>
#include <kernel/proc.h>
#include <kernlib/string.h>

int ticks;

/**
 * clock_handler
 */
void clock_handler(int irq)
{
    ticks++;

    /* There is two stages - in kernel intializing or in process running.
     * Some operation shouldn't be valid in kernel intializing stage.
     * added by xw, 18/6/1
     */
    if (kernel_initial == 1) {
        return;
    }
    irq = 0;
    p_proc_current->task.ticks--;
    sys_wakeup(&ticks);
}

/**
 * milli_delay
 */
void milli_delay(int milli_sec)
{
    int t = sys_get_ticks();

    while (((sys_get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

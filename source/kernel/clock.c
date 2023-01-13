/**
 * @file clock.c
 * @author Forrest Yu
 * @date 2005
 */

#include "common/type.h"
#include "kernel/global.h"
#include "kernel/proto.h"
#include "lib/string.h"
#include "software_define/kern_const.h"
#include "software_define/proc_define.h"
#include "software_define/protect_define.h"

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
    int t = get_ticks();

    while (((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

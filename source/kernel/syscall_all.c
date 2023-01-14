#include <kernel/syscall.h>
#include <kernel/vfs.h>

system_call sys_call_table[NR_SYS_CALL] = {
    sys_get_ticks,    // 1st
    sys_get_pid,      // add by visual 2016.4.6
    sys_kmalloc,      // add by visual 2016.4.6
    sys_kmalloc_4k,   // add by visual 2016.4.7
    sys_malloc,       // add by visual 2016.4.7	//5th
    sys_malloc_4k,    // add by visual 2016.4.7
    sys_free,         // add by visual 2016.4.7
    sys_free_4k,      // add by visual 2016.4.7
    sys_fork,         // add by visual 2016.4.8
    sys_pthread,      // add by visual 2016.4.11	//10th
    sys_display_int,  // add by visual 2016.5.16
    sys_display_str,  // add by visual 2016.5.16
    sys_exec,         // add by visual 2016.5.16
    sys_yield,        // added by xw
    sys_sleep,        // added by xw				//15th
    sys_print_E,      // added by xw
    sys_print_F,      // added by xw
    sys_open,         // added by xw, 18/6/18
    sys_close,        // added by xw, 18/6/18
    sys_read,         // added by xw, 18/6/18		//20th
    sys_write,        // added by xw, 18/6/18
    sys_lseek,        // added by xw, 18/6/18
    sys_unlink,       // added by xw, 18/6/19		//23th
    sys_create,       // added by mingxuan 2019-5-17
    sys_delete,       // added by mingxuan 2019-5-17
    sys_opendir,      // added by mingxuan 2019-5-17
    sys_createdir,    // added by mingxuan 2019-5-17
    sys_deletedir,    // added by mingxuan 2019-5-17
    sys_set_color     // added by tastror 2019-5-17
};
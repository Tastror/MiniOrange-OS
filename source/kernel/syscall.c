﻿#include <kernel/syscall.h>
#include <kernel/vfs.h>
#include <kernel/memman.h>
#include <kernel/network.h>
#include <kernel/pagepte.h>
#include <kernel/proc.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

system_call syscall_table[NR_SYS_CALL] = {
    [0] = sys_get_ticks,
    [1] = sys_get_pid,       // add by visual 2016.4.6
    [2] = sys_kmalloc,       // add by visual 2016.4.6
    [3] = sys_kmalloc_4k,    // add by visual 2016.4.7
    [4] = sys_malloc,        // add by visual 2016.4.7
    [5] = sys_malloc_4k,     // add by visual 2016.4.7
    [6] = sys_free,          // add by visual 2016.4.7
    [7] = sys_free_4k,       // add by visual 2016.4.7
    [8] = sys_fork,          // add by visual 2016.4.8
    [9] = sys_pthread,       // add by visual 2016.4.11
    [10] = sys_display_int,  // add by visual 2016.5.16
    [11] = sys_display_str,  // add by visual 2016.5.16
    [12] = sys_exec,         // add by visual 2016.5.16
    [13] = sys_yield,        // added by xw
    [14] = sys_sleep,        // added by xw
    [15] = sys_print_E,      // added by xw
    [16] = sys_print_F,      // added by xw
    [17] = sys_open,         // added by xw, 18/6/18
    [18] = sys_close,        // added by xw, 18/6/18
    [19] = sys_read,         // added by xw, 18/6/18
    [20] = sys_write,        // added by xw, 18/6/18
    [21] = sys_lseek,        // added by xw, 18/6/18
    [22] = sys_unlink,       // added by xw, 18/6/19
    [23] = sys_create,       // added by mingxuan 2019-5-17
    [24] = sys_delete,       // added by mingxuan 2019-5-17
    [25] = sys_opendir,      // added by mingxuan 2019-5-17
    [26] = sys_createdir,    // added by mingxuan 2019-5-17
    [27] = sys_deletedir,    // added by mingxuan 2019-5-17
    [28] = sys_set_color,    // added by tastror 2023-1-10
    [29] = sys_net_test,
    [30] = sys_net_arp,
    [31] = sys_net_receive
};

struct memfree *memarg = 0;

/**
 * sys_get_ticks
 * add by visual 2016.4.6
 */
int sys_get_ticks()
{
    return ticks;
}

/*======================================================================*
                           sys_get_pid        add by visual 2016.4.6
 *======================================================================*/
int sys_get_pid()
{
    return p_proc_current->task.pid;
}

/*======================================================================*
                           sys_kmalloc        add by visual 2016.4.6
 *======================================================================*/
void *sys_kmalloc(int size)
{  // edit by visual 2015.5.9
    return (void *)(do_kmalloc(size));
}

/*======================================================================*
                           sys_kmalloc_4k        add by visual 2016.4.7
 *======================================================================*/
void *sys_kmalloc_4k()
{
    return (void *)(do_kmalloc_4k());
}

/*======================================================================*
                           sys_malloc        edit by visual 2016.5.4
 *======================================================================*/
void *sys_malloc(int size)
{
    int vir_addr, AddrLin;
    vir_addr = vmalloc(size);

    for (AddrLin = vir_addr; AddrLin < vir_addr + size; AddrLin += num_4B)  // 一个字节一个字节处理
    {
        lin_mapping_phy(
            AddrLin,                   // 线性地址                    //add by visual 2016.5.9
            MAX_UNSIGNED_INT,          // 物理地址                        //edit by visual 2016.5.19
            p_proc_current->task.pid,  // 进程pid                //edit by visual 2016.5.19
            PG_P | PG_USU | PG_RWW,    // 页目录的属性位
            PG_P | PG_USU | PG_RWW
        );  // 页表的属性位
    }
    return (void *)vir_addr;
}


/*======================================================================*
                           sys_malloc_4k        edit by visual 2016.5.4
 *======================================================================*/
void *sys_malloc_4k()
{
    int vir_addr, AddrLin;
    vir_addr = vmalloc(num_4K);

    for (AddrLin = vir_addr; AddrLin < vir_addr + num_4K; AddrLin += num_4K)  // 一页一页处理(事实上只有一页，而且一定没有填写页表，页目录是否填写不确定)
    {
        lin_mapping_phy(
            AddrLin,                   // 线性地址                    //add by visual 2016.5.9
            MAX_UNSIGNED_INT,          // 物理地址
            p_proc_current->task.pid,  // 进程pid                    //edit by visual 2016.5.19
            PG_P | PG_USU | PG_RWW,    // 页目录的属性位
            PG_P | PG_USU | PG_RWW
        );  // 页表的属性位
    }
    return (void *)vir_addr;
}


/*======================================================================*
                           sys_free        add by visual 2016.4.7
 *======================================================================*/
int sys_free(void *arg)
{
    memarg = (struct memfree *)arg;
    return do_free(memarg->addr, memarg->size);
}

/*======================================================================*
                           sys_free_4k        edit by visual 2016.5.9
 *======================================================================*/
int sys_free_4k(void *AddrLin)
{                  // 线性地址可以不释放，但是页表映射关系必须清除！
    int phy_addr;  // add by visual 2016.5.9

    phy_addr = get_page_phy_addr(p_proc_current->task.pid, (int)AddrLin);  // 获取物理页的物理地址        //edit by visual 2016.5.19
    lin_mapping_phy(
        (int)AddrLin,              // 线性地址
        phy_addr,                  // 物理地址
        p_proc_current->task.pid,  // 进程pid            //edit by visual 2016.5.19
        PG_P | PG_USU | PG_RWW,    // 页目录的属性位
        0 | PG_USU | PG_RWW
    );  // 页表的属性位
    return do_free_4k(phy_addr);
}


/*======================================================================*
                           sys_display_int        add by visual 2016.5.16
用户用的打印函数
 *======================================================================*/
void sys_display_int(int arg)
{
    kern_display_integer(arg);
    return;
}

/*======================================================================*
                           sys_display_str        add by visual 2016.5.16
用户用的打印函数
 *======================================================================*/
void sys_display_str(char *arg)
{
    kern_display_string(arg);
    return;
}

void sys_set_color(int color)
{
    kern_set_color(color);
    return;
}

void sys_net_test(u8 *args)
{
    kern_net_test(args);
    return;
}

void sys_net_arp()
{
    kern_net_arp();
    return;
}

char *sys_net_receive()
{
    return kern_net_receive();
}
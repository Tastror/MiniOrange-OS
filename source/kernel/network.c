#include <kernel/network.h>

#include <kernlib/stdio.h>
#include <kernlib/string.h>

u8* e1000_save_buff[E1000_PACK_BUFF_SAVE_SIZE];
u16 e1000_save_len;

// syscall 向量表跳转函数 sys_net_test 的处理函数
//（内核调用）
void kern_net_test(u8 *args)
{
    // args 前两个字节 (u16) 用来存大小
    // args 后面的字节用来存内容

    u16 len = *(u16 *)args;
    u8 *data = args + 2;

    kern_set_color(CYAN);
    kprintf("This is for net test: %u, %s\n", len, (char *)data);
    kern_set_color(WHITE);

    memcpy((void*)e1000_save_buff, (void*)data, len);
    e1000_save_len = len;

    kprintf("e1000 begin to send pack now...\n");

    return;
}
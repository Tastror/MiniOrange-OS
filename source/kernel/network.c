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

    // 触发中断，进入 e1000_send_pack_handler()
    __asm__ __volatile__(
        "int %0" : : "i"(INT_VECTOR_E1000_SEND_PACK)
    );

    return;
}

// 发送中断处理函数
// 当准备好一个 pack 后，要求网卡发送时触发该中断
void e1000_send_pack_handler()
{
    // use int INT_VECTOR_E1000_SEND_PACK
    // and put your data in pack

    __asm__ __volatile__(
        "pushfl\n\tpushal\n\t"
    );

    kprintf("e1000 begin to send pack now...\n");

    __asm__ __volatile__(
        "popal\n\tpopfl\n\t"
    );
}


// 接收中断处理函数
// 将网卡的 pack 保存到数组里
void e1000_receive_pack_handler()
{
    // use int INT_VECTOR_E1000_RECEIVE_PACK
    // and put your data in pack

    __asm__ __volatile__(
        "pushfl\n\tpushal\n\t"
    );

    kprintf("begin to receive e1000's pack now...\n");

    __asm__ __volatile__(
        "popal\n\tpopfl\n\t"
    );
}
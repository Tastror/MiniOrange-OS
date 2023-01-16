#include <userlib/stdlib.h>
#include <kernlib/string.h>
#include <kernel/syscall.h>
#include <device/ip.h>
#include <device/arp.h>

void delay(int time)
{
    int i, j, k;
    for (k = 0; k < time; k++) {
        // for (i = 0; i < 10000; i++) {  // for Virtual PC
        for (i = 0; i < 10; i++) {  // for Bochs
            for (j = 0; j < 10000; j++) {}
        }
    }
}

void net_test_shell(u16 len, u8 *data)
{
    u8 mem[E1000_PACK_BUFF_SAVE_SIZE];
    memcpy((void *)(mem + 2), (void *)data, len);
    *(u16 *)mem = len;
    net_test(mem);
}
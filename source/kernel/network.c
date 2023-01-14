#include <kernel/network.h>

#include <kernlib/stdio.h>

void kern_net_test()
{
    kern_set_color(CYAN);
    kprintf("This is for net test\n");
    kern_set_color(WHITE);
    return;
}
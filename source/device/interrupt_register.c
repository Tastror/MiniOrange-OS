#include <device/interrupt_register.h>

#include <kernlib/stdio.h>

struct device_interrupt device_interrupt_table[DEVICE_INTERRUPT_SIZE];
u8                      device_interrupt_num = 0;

void register_device_interrupt(u8 interrupt_num, u8 desc_type, int_handler handler_func, u8 privilege)
{
    if (device_interrupt_num > DEVICE_INTERRUPT_SIZE) {
        kprintf("error! too much device interrupt!\n");
        return;
    }
    struct device_interrupt *now = &device_interrupt_table[device_interrupt_num];
    now->interrupt_num = interrupt_num;
    now->desc_type = desc_type;
    now->handler_func = handler_func;
    now->privilege = privilege;
    device_interrupt_num++;
    return;
}
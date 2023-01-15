#ifndef _INTERRUPT_REGISTER_H_
#define _INTERRUPT_REGISTER_H_

#include <common/type.h>
#include <define/define.h>

#define DEVICE_INTERRUPT_SIZE 10

struct device_interrupt {
    u8 interrupt_num;
    u8 desc_type;
    int_handler handler_func;
    u8 privilege;
};

extern struct device_interrupt device_interrupt_table[DEVICE_INTERRUPT_SIZE];
extern u8 device_interrupt_num;

void register_device_interrupt(u8 interrupt_num, u8 desc_type, int_handler func, u8 privilege);

#endif
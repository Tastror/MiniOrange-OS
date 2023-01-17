
/**
 * @file start.c
 * @author Forrest Yu
 * @date 2005
 */

#include <kernlib/stdio.h>
#include <kernlib/string.h>
#include <device/pci.h>
#include <device/interrupt_register.h>
#include <kernel/kernel.h>
#include <kernel/protect.h>

void cstart()
{
    // kprintf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    kprintf("-----\"cstart\" begins-----\n");

    // 将 LOADER 中的 GDT 复制到新的 GDT 中
    memcpy(&gdt,                               // New GDT
           (void *)(*((u32 *)(&gdt_ptr[2]))),  // Base  of Old GDT
           *((u16 *)(&gdt_ptr[0])) + 1         // Limit of Old GDT
    );
    // gdt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sgdt 以及 lgdt 的参数。
    u16 *p_gdt_limit = (u16 *)(&gdt_ptr[0]);
    u32 *p_gdt_base = (u32 *)(&gdt_ptr[2]);
    *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32)&gdt;

    // idt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sidt 以及 lidt 的参数。
    u16 *p_idt_limit = (u16 *)(&idt_ptr[0]);
    u32 *p_idt_base = (u32 *)(&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
    *p_idt_base = (u32)&idt;

    kern_set_color(CYAN);
    device_interrupt_num = 0;
    init_prot();
    kern_set_color(WHITE);

    kprintf("-----\"cstart\" finished-----\n");
}
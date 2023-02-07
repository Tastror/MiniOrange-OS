
/**
 * @file protect.c
 * @author Forrest Yu,
 * @date 2005
 */

#include <kernel/protect.h>

#include <device/interrupt_register.h>
#include <device/e1000.h>
#include <kernel/interrupt.h>
#include <kernel/kernel.h>
#include <kernel/proc.h>
#include <kernel/syscall.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

TSS tss;

/* 本文件内函数声明 */
static void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege);
static void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute);

/* 中断处理函数, implement in core.asm */
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();
void syscall_handler();

void init_prot()
{
    init_8259A();

    // 添加 device 初始化时提供的中断。
    // 因为 device 不依赖 kernel，所以写到了一个结构体里，在这个时候处理
    for (int i = 0; i < device_interrupt_num; ++i) {
        struct device_interrupt *now = &device_interrupt_table[i];
        init_idt_desc(now->interrupt_num, now->desc_type, now->handler_func, now->privilege);
        kprintf("add device interrupt num %u\n", now->interrupt_num);
    }

    // 全部初始化成中断门(没有陷阱门)
    init_idt_desc(INT_VECTOR_DIVIDE, DA_386IGate, divide_error, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_DEBUG, DA_386IGate, single_step_exception, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_NMI, DA_386IGate, nmi, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_BREAKPOINT, DA_386IGate, breakpoint_exception, PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_OVERFLOW, DA_386IGate, overflow, PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_BOUNDS, DA_386IGate, bounds_check, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_INVAL_OP, DA_386IGate, inval_opcode, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_NOT, DA_386IGate, copr_not_available, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_SEG, DA_386IGate, copr_seg_overrun, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_INVAL_TSS, DA_386IGate, inval_tss, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_SEG_NOT, DA_386IGate, segment_not_present, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_STACK_FAULT, DA_386IGate, stack_exception, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_PROTECTION, DA_386IGate, general_protection, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_PAGE_FAULT, DA_386IGate, page_fault, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_ERR, DA_386IGate, copr_error, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_SYS_CALL, DA_386IGate, syscall_handler, PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_E1000, DA_386IGate, e1000_receive_pack_handler, PRIVILEGE_KRNL);

    /* 修改显存描述符 */  // add by visual 2016.5.12
    init_descriptor(&gdt[INDEX_VIDEO], K_PHY2LIN(0x0B8000), 0x0ffff, DA_DRW | DA_DPL3);

    /* 填充 GDT 中 TSS 这个描述符 */
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_KERNEL_DS;
    init_descriptor(&gdt[INDEX_TSS], vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss), sizeof(tss) - 1, DA_386TSS);
    tss.iobase = sizeof(tss); /* 没有I/O许可位图 */

    // 填充 GDT 中进程的 LDT 的描述符
    PROCESS *p_proc = proc_table;
    u16      selector_ldt = INDEX_LDT_FIRST << 3;

    for (int i = 0; i < NR_PCBS; i++) {  // edit by visual 2016.4.5
        init_descriptor(
            &gdt[selector_ldt >> 3],
            vir2phys(seg2phys(SELECTOR_KERNEL_DS), proc_table[i].task.ldts),
            LDT_SIZE * sizeof(DESCRIPTOR) - 1,
            DA_LDT
        );
        p_proc++;
        selector_ldt += 1 << 3;
    }
}


/*======================================================================*
                             init_idt_desc
 *----------------------------------------------------------------------*
 初始化 386 中断门
 *======================================================================*/
void init_idt_desc(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege)
{
    GATE *p_gate = &idt[vector];
    u32   base = (u32)handler;
    p_gate->offset_low = base & 0xFFFF;
    p_gate->selector = SELECTOR_KERNEL_CS;
    p_gate->dcount = 0;
    p_gate->attr = desc_type | (privilege << 5);
    p_gate->offset_high = (base >> 16) & 0xFFFF;
}


/*======================================================================*
                           seg2phys
 *----------------------------------------------------------------------*
 由段名求绝对地址
 *======================================================================*/
u32 seg2phys(u16 seg)
{
    DESCRIPTOR *p_dest = &gdt[seg >> 3];

    return (p_dest->base_high << 24) | (p_dest->base_mid << 16) | (p_dest->base_low);
}

/*======================================================================*
                           init_descriptor
 *----------------------------------------------------------------------*
 初始化段描述符
 *======================================================================*/
static void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute)
{
    p_desc->limit_low = limit & 0x0FFFF;      // 段界限 1		(2 字节)
    p_desc->base_low = base & 0x0FFFF;        // 段基址 1		(2 字节)
    p_desc->base_mid = (base >> 16) & 0x0FF;  // 段基址 2		(1 字节)
    p_desc->attr1 = attribute & 0xFF;         // 属性 1
    p_desc->limit_high_attr2 = ((limit >> 16) & 0x0F) |
                               ((attribute >> 8) & 0xF0);  // 段界限 2 + 属性 2
    p_desc->base_high = (base >> 24) & 0x0FF;              // 段基址 3		(1 字节)
}

/*======================================================================*
                            exception_handler
 *----------------------------------------------------------------------*
 异常处理
 *======================================================================*/
void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
    int  i;
    char err_description[][64] = {"#DE Divide Error", "#DB RESERVED", "—  NMI Interrupt", "#BP Breakpoint", "#OF Overflow", "#BR BOUND Range Exceeded", "#UD Invalid Opcode (Undefined Opcode)", "#NM Device Not Available (No Math Coprocessor)", "#DF Double Fault", "    Coprocessor Segment Overrun (reserved)", "#TS Invalid TSS", "#NP Segment Not Present", "#SS Stack-Segment Fault", "#GP General Protection", "#PF Page Fault", "—  (Intel reserved. Do not use.)", "#MF x87 FPU Floating-Point Error (Math Fault)", "#AC Alignment Check", "#MC Machine Check", "#XF SIMD Floating-Point Exception"};

    /* 通过打印空格的方式清空屏幕的前五行，并把 disp_pos 清零 */
    disp_pos = 0;
    for (i = 0; i < 80 * 5; i++) {
        kern_display_string(" ");
    }
    disp_pos = 0;

    kern_set_color(MAKE_COLOR(GREY, RED));
    kern_display_string("Exception! --> ");
    kern_display_string(err_description[vec_no]);
    kern_display_string("\n\n");
    kern_display_string("EFLAGS:");
    kern_display_integer(eflags);
    kern_display_string("CS:");
    kern_display_integer(cs);
    kern_display_string("EIP:");
    kern_display_integer(eip);

    if (err_code != 0xFFFFFFFF) {
        kern_display_string("Error code:");
        kern_display_integer(err_code);
    }

    // added by xw, 18/12/19
    kern_display_string("\n");
    kern_set_color(WHITE);

    // added by xw, 18/12/19
    p_proc_current->task.stat = KILLED;
}

/*======================================================================*
                                                        divide error handler
 *======================================================================*/
// used for testing if a exception handler can be interrupted rightly, so it's
// not a real divide_error handler now. added by xw, 18/12/22
void divide_error_handler()
{
    int vec_no, err_code, eip, cs, eflags;
    int i, j;

    asm volatile("mov 8(%%ebp), %0\n\t"   // get vec_no from stack
                 "mov 12(%%ebp), %1\n\t"  // get err_code from stack
                 "mov 16(%%ebp), %2\n\t"  // get eip from stack
                 "mov 20(%%ebp), %3\n\t"  // get cs from stack
                 "mov 24(%%ebp), %4\n\t"  // get eflags from stack
                 : "=r"(vec_no), "=r"(err_code), "=r"(eip),
                   "=r"(cs), "=r"(eflags)
    );
    exception_handler(vec_no, err_code, eip, cs, eflags);

    while (1) {
        kern_display_string("Loop in divide error handler...\n");

        i = 100;
        while (--i) {
            j = 1000;
            while (--j) {}
        }
    }
}

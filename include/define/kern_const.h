/**
 * const.h
 *
 * Forrest Yu
 * 2005
 */

#ifndef _ORANGES_KERN_CONST_H_
#define _ORANGES_KERN_CONST_H_

/* Color */
#define BLACK       0x0 /* 0000 */
#define DARKBLUE    0x1 /* 0001 */
#define DARKGREEN   0x2 /* 0010 */
#define DARKRED     0x4 /* 0100 */
#define DARKCYAN    0x3 /* 0011 */
#define DARKMAGENTA 0x5 /* 0101 */
#define DARKYELLOW  0x6 /* 0110 */
#define GREY        0x7 /* 0111 */

#define DARKGREY (0x8 | BLACK)
#define BLUE     (0x8 | DARKBLUE)
#define GREEN    (0x8 | DARKGREEN)
#define RED      (0x8 | DARKRED)
#define CYAN     (0x8 | DARKCYAN)
#define MAGENTA  (0x8 | DARKMAGENTA)
#define YELLOW   (0x8 | DARKYELLOW)
#define WHITE    (0x8 | GREY)

// e.g. MAKE_COLOR(BLUE, RED)
#define MAKE_COLOR(x, y) (((x) << 4) | (y)) /* MAKE_COLOR(Background, Foreground) */

/* GDT 和 IDT 中描述符的个数 */
#define GDT_SIZE 128
#define IDT_SIZE 256

/* 权限 */
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_TASK 1
#define PRIVILEGE_USER 3

/* 8259A interrupt controller ports. */
#define INT_M_CTL     0x20 /* I/O port for interrupt controller         <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints   <Master> */
#define INT_S_CTL     0xA0 /* I/O port for second interrupt controller  <Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints   <Slave>  */

/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40     /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43     /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34     /* 00-11-010-0 : Counter0 - LSB then MSB - rate generator - binary */
#define TIMER_FREQ     1193182L /* clock frequency for timer in PC and AT */
#define HZ             100      /* clock freq (software settable on IBM-PC) */

/* Hardware interrupts */
#define NR_IRQ        16 /* Number of IRQs */
#define CLOCK_IRQ     0
#define KEYBOARD_IRQ  1
#define CASCADE_IRQ   2 /* cascade enable for 2nd AT controller */
#define ETHER_IRQ     3 /* default ethernet interrupt vector */
#define SECONDARY_IRQ 3 /* RS232 interrupt vector for port 2 */
#define RS232_IRQ     4 /* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ   5 /* xt winchester */
#define FLOPPY_IRQ    6 /* floppy disk */
#define PRINTER_IRQ   7
#define AT_WINI_IRQ   14  /* at winchester */
#define MOUSE_IRQ     12  // added by mingxuan 2019-5-19

/* system call */
#define NR_SYS_CALL 64  // modified by tastror 2022-12

/* TTY */
// added by mingxuan 2019-5-19
#define NR_CONSOLES 1 /* consoles */

/* 页表相关 */
#define PageTblNumAddr    0x500     // 页表数量放在这个位置,必须与 load.inc 中一致  // add by visual 2016.5.11
#define KernelPageTblAddr 0x200000  // 内核页表物理地址，必须与 load.inc 中一致  // add by visual 2016.5.17
// Page directory and page table constants.
#define NPDENTRIES 1024  // page directory entries per page directory
#define NPTENTRIES 1024  // page table entries per page table

#define PGSIZE  4096  // bytes mapped by a page
#define PGSHIFT 12    // log2(PGSIZE)

#define PTSIZE  (PGSIZE * NPTENTRIES)  // bytes mapped by a page directory entry
#define PTSHIFT 22                     // log2(PTSIZE)

#define PTXSHIFT 12  // offset of PTX in a linear address
#define PDXSHIFT 22  // offset of PDX in a linear address
/**
 * 线性地址描述
 * // edit by visual 2016.5.25
 */
#define KernelSize        0x800000                             // 内核的大小  // add by visual 2016.5.10
#define K_PHY2LIN(x)      ((x) + 0xC0000000)                   // 内核中物理地址转线性地址  // add by visual 2016.5.10
#define K_LIN2PHY(x)      ((x) - 0xC0000000)                   // added by xw, 18/8/27
#define num_4B            0x4                                  // 4B 大小
#define num_1K            0x400                                // 1k 大小
#define num_4K            0x1000                               // 4k 大小
#define num_4M            0x400000                             // 4M 大小

#define TextLinBase       ((u32)0x0)                           // 0, 进程代码的起始地址，这是参考值，具体以 elf 描述为准
#define TextLinLimitMAX   (TextLinBase + 0x20000000)           // 521M, 大小：512M，这是参考值，具体以 elf 描述为准，
#define DataLinBase       TextLinLimitMAX                      // 521M, 进程数据的起始地址，这是参考值，具体以 elf 描述为准
#define DataLinLimitMAX   (DataLinBase + 0x20000000)           // 1G, 大小：512M，这是参考值，具体以 elf 描述为准，但是代码和数据长度总和不能超过这个值
#define VpageLinBase      DataLinLimitMAX                      // 1G, 保留内存起始地址
#define VpageLinLimitMAX  (VpageLinBase + 0x8000000 - num_4K)  // 1G + 128M - 4K, 大小：128M - 4k
#define SharePageBase     VpageLinLimitMAX                     // 1G + 128M - 4K, 共享页线性地址，执行 fork\pthread 的时候用，共享页必须 4K 对齐
#define SharePageLimit    (SharePageBase + num_4K)             // 1G + 128M, 大小：4k
#define HeapLinBase       SharePageLimit                       // 1G + 128M, 堆的起始地址
#define HeapLinLimitMAX   (HeapLinBase + 0x40000000)           // 2G + 128M, 大小：1G
#define StackLinLimitMAX  HeapLinLimitMAX                      // 2G + 128M, 栈的大小：1G - 128M - 4M - 4K - 4B（注意栈的基址和界限方向）// edit by network 2023.1.17
// 3G 是一个分割点，从这开始自上向下的定义空间，剩下的空间留给进程栈使用
#define KernelLinBase     0xC0000000                           // 3G, 内核线性起始地址（有 0x30400 的偏移）
#define KernelLinLimitMAX (KernelLinBase + 0x40000000)         // 4G, 大小：1G
#define ArgLinLimitMAX    KernelLinBase                        // 3G, = (ArgLinBase + 0x1000)，大小：4K
#define ArgLinBase        (KernelLinBase - num_4K)             // 3G - 4K, 参数存放位置起始地址，放在 3G 前，暂时还没没用到
#define MMIOLIM           (ArgLinBase)                         // 3G - 4K, add by network 2023.1.17
#define MMIOBASE          (MMIOLIM - PTSIZE)                   // 3G - 4M - 4K, add by network 2023.1.17
#define StackLinBase      (MMIOBASE - num_4B)                  // 3G - 4M - 4K - 4B, 栈的起始地址，放在参数位置之前（注意堆栈的增长方向） // edit by network 2023.1.17
// #define ShareTblLinAddr     (KernelLinLimitMAX - 0x1000)        // 公共临时共享页，放在内核最后一个页表的最后一项上

/*
 * 目前线性地址布局  // edit by visual 2016.5.25
 *
 * 进程代码                 0 ~ 512M ,限制大小为 512M
 * 进程数据                 512M ~ 1G，限制大小为 512M
 * 进程保留内存（以后可能存放虚页表和其他一些信息）  1G ~ (1G + 128M)，限制大小为 128M,共享页放在这个位置
 * 进程堆                   (1G + 128M) ~ (2G + 128M)，限制大小为 1G
 * 进程栈                   (2G + 128M) ~ (3G - 4M - 4K - 4B)，限制大小为 1G - 128M - 4M - 4K - 4B // edit by network 2023.1.17
 * 空白区域                 (3G - 4M - 4K - 4B) ~ (3G - 4M - 4K) // edit by network 2023.1.17
 * MMIO设备寄存器映射区域     (3G - 4M - 4K) ~ (3G - 4K)，限制大小为 4M // edit by network 2023.1.17
 * 进程参数                 (3G - 4K) ~ 3G，限制大小为 4K
 * 内核                    3G ~ 4G，限制大小为 1G
 */

/* 分页机制常量的定义，必须与 load.inc 中一致 */  // add by visual 2016.4.5
#define PG_P   0x001                              // 页存在属性位
#define PG_RWR 0x000                              // R/W 属性位值，读/执行
#define PG_RWW 0x002                              // R/W 属性位值，读/写/执行
#define PG_USS 0x000                              // U/S 属性位值，系统级
#define PG_USU 0x004                              // U/S 属性位值，用户级
#define PG_PS  0x040                              // PS 属性位值，4K 页
#define PG_PWT 0x008                              // Write-Through
#define PG_PCD 0x010                              // Cache-Disable

/* AT keyboard */
/* 8042 ports */
// added by mingxuan 2019-5-19

/**
 * I/O port for keyboard data
 *    Read : Read Output Buffer
 *    Write: Write Input Buffer
 *             (8042 Data & 8048 Command)
 */
#define KB_DATA 0x60
/**
 * I/O port for keyboard command
 *    Read : Read Status Register
 *    Write: Write Input Buffer
 *           (8042 Command)
 */
#define KB_CMD               0x64
#define KB_STA               0x64
#define KEYSTA_SEND_NOTREADY 0x02
#define KBSTATUS_IBF         0x02
#define KBSTATUS_OBF         0x01

#define wait_KB_write() while (in_byte(KB_STA) & KBSTATUS_IBF)
#define wait_KB_read()  while (in_byte(KB_STA) & KBSTATUS_OBF)

#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE          0x47

#define KEYCMD_SENDTO_MOUSE    0xd4
#define MOUSECMD_ENABLE        0xf4
#define KBCMD_EN_MOUSE_INTFACE 0xa8

#define LED_CODE 0xED
#define KB_ACK   0xFA

/* VGA */
// added by mingxuan 2019-5-19
#define CRTC_ADDR_REG 0x3D4   /* CRT Controller Registers - Addr Register */
#define CRTC_DATA_REG 0x3D5   /* CRT Controller Registers - Data Register */
#define START_ADDR_H  0xC     /* reg index of video mem start addr (MSB) */
#define START_ADDR_L  0xD     /* reg index of video mem start addr (LSB) */
#define CURSOR_H      0xE     /* reg index of cursor position (MSB) */
#define CURSOR_L      0xF     /* reg index of cursor position (LSB) */
#define V_MEM_BASE    0xB8000 /* base of color video memory */
#define V_MEM_SIZE    0x8000 /* 64K: B8000H -> BFFFFH */

#define STD_IN  0
#define STD_OUT 1
#define STD_ERR 2

#endif /* _ORANGES_KERN_CONST_H_ */
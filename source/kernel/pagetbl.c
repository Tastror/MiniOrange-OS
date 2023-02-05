/**
 * 页式管理相关代码 add by visual 2016.4.19
 */

#include <kernel/kernel.h>
#include <kernel/memman.h>
#include <kernel/pagepte.h>
#include <kernel/proc.h>
#include <kernlib/assert.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

u32 cr3_ready;
u32 cr2_save;
u32 cr2_count = 0;

/**
 * added by xw, 17/12/11
 * switch the page directory table after schedule() is called
 */
void switch_pde()
{
    cr3_ready = p_proc_current->task.cr3;
}

/**
 * add by visual 2016.4.19
 * 页表初始化函数
 * 该函数只初始化了进程的高端（内核端）地址页表
 */
u32 init_page_pte(u32 pid)
{

    u32 AddrLin, pde_addr_phy_temp, err_temp;

    pde_addr_phy_temp = do_kmalloc_4k();  // 为页目录申请一页

    memset((void *)K_PHY2LIN(pde_addr_phy_temp), 0, num_4K);  // add by visual 2016.5.26

    if (pde_addr_phy_temp < 0 || (pde_addr_phy_temp & 0x3FF) != 0)  // add by visual 2016.5.9
    {
        kern_set_color(MAKE_COLOR(GREY, RED));
        kern_display_string("init_page_pte Error: pde_addr_phy_temp");
        kern_set_color(WHITE);
        return -1;
    }

    proc_table[pid].task.cr3 = pde_addr_phy_temp;  // 初始化了进程表中 cr3 寄存器变量，属性位暂时不管

    u32 phy_addr = 0;

    // 只初始化内核部分，3G 后的线性地址映射到物理地址开始处
    for (AddrLin = KernelLinBase, phy_addr = 0; AddrLin < KernelLinBase + KernelSize; AddrLin += num_4K, phy_addr += num_4K) {
        err_temp = lin_mapping_phy(
            AddrLin,                 // 线性地址  // add by visual 2016.5.9
            phy_addr,                // 物理地址
            pid,                     // 进程 pid  // edit by visual 2016.5.19
            PG_P | PG_USU | PG_RWW,  // 页目录的属性位（用户权限）  // edit by visual 2016.5.26
            PG_P | PG_USS | PG_RWW   // 页表的属性位（系统权限）  // edit by visual 2016.5.17
        );
        if (err_temp != 0) {
            kern_set_color(MAKE_COLOR(GREY, RED));
            kern_display_string("init_page_pte Error: lin_mapping_phy");
            kern_set_color(WHITE);
            return -1;
        }
    }

    return 0;
}

/**
 * edit by visual 2016.5.9
 * 缺页中断处理函数
 */
void page_fault_handler(
    u32 vec_no,    // 异常编号，此时应该是 14，代表缺页异常
    u32 err_code,  // 错误码
    u32 eip,       // 导致缺页的指令的线性地址
    u32 cs,        // 发生错误时的代码段寄存器内容
    u32 eflags     // 时发生错误的标志寄存器内容
)
{
    u32 pde_addr_phy_temp;
    u32 pte_addr_phy_temp;
    u32 cr2;

    cr2 = read_cr2();

    // if page fault happens in kernel, it's an error.
    if (kernel_initial == 1) {
        kern_display_string("\n");
        kern_set_color(MAKE_COLOR(GREY, RED));
        kern_display_string("Page Fault\n");
        kern_display_string("eip=");  // 灰底红字
        kern_display_integer(eip);
        kern_display_string("eflags=");
        kern_display_integer(eflags);
        kern_display_string("cs=");
        kern_display_integer(cs);
        kern_display_string("err_code=");
        kern_display_integer(err_code);
        kern_display_string("Cr2=");  // 灰底红字
        kern_display_integer(cr2);
        kern_set_color(WHITE);
        halt();
    }

    // 获取该进程页目录物理地址
    pde_addr_phy_temp = get_pde_phy_addr(p_proc_current->task.pid);
    // 获取该线性地址对应的页表的物理地址
    pte_addr_phy_temp = get_pte_phy_addr(p_proc_current->task.pid, cr2);

    if (cr2 == cr2_save) {
        cr2_count++;
        if (cr2_count == 5) {
            kern_display_string("\n");
            kern_set_color(MAKE_COLOR(GREY, RED));
            kern_display_string("Page Fault\n");
            kern_display_string("eip=");  // 灰底红字
            kern_display_integer(eip);
            kern_display_string("eflags=");
            kern_display_integer(eflags);
            kern_display_string("cs=");
            kern_display_integer(cs);
            kern_display_string("err_code=");
            kern_display_integer(err_code);
            kern_display_string("Cr2=");  // 灰底红字
            kern_display_integer(cr2);
            kern_display_string("Cr3=");
            kern_display_integer(p_proc_current->task.cr3);
            // 获取页目录中填写的内容
            kern_display_string("Pde=");
            kern_display_integer(*((u32 *)K_PHY2LIN(pde_addr_phy_temp) + get_pde_index(cr2)));
            // 获取页表中填写的内容
            kern_display_string("Pte=");
            kern_display_integer(*((u32 *)K_PHY2LIN(pte_addr_phy_temp) + get_pte_index(cr2)));
            kern_set_color(WHITE);
            halt();
        }
    } else {
        cr2_save = cr2;
        cr2_count = 0;
    }

    if (0 == pte_exist(pde_addr_phy_temp, cr2)) {  // 页表不存在
        // kern_set_color(MAKE_COLOR(GREY, RED));  // 灰底红字
        // kern_display_string("[Pde Fault!]");
        (*((u32 *)K_PHY2LIN(pde_addr_phy_temp) + get_pde_index(cr2))) |= PG_P;
        // kern_display_string("[Solved]");
        // kern_set_color(WHITE);
    } else {  // 只是缺少物理页
        // kern_set_color(MAKE_COLOR(GREY, RED));  // 灰底红字
        // kern_display_string("[Pte Fault!]");
        (*((u32 *)K_PHY2LIN(pte_addr_phy_temp) + get_pte_index(cr2))) |= PG_P;
        // kern_display_string("[Solved]");
        // kern_set_color(WHITE);
    }
    refresh_page_cache();
}

/***************************地址转换过程***************************
 *
 *第一步，CR3包含着页目录的起始地址，用32位线性地址的最高10位A31~A22作为页目录的页目录项的索引，
 *将它乘以4，与CR3中的页目录的起始地址相加，形成相应页表的地址。
 *
 *第二步，从指定的地址中取出32位页目录项，它的低12位为0，这32位是页表的起始地址。
 *用32位线性地址中的A21~A12位作为页表中的页面的索引，将它乘以4，与页表的起始地址相加，形成32位页面地址。
 *
 *第三步，将A11~A0作为相对于页面地址的偏移量，与32位页面地址相加，形成32位物理地址。
 *********************************************************************** */

/**
 * add by visual 2016.4.28
 * 由 线性地址 得到 页目录项编号
 */
inline u32 get_pde_index(u32 AddrLin)
{
    return (AddrLin >> 22);  // 高10位A31~A22
}

/**
 * add by visual 2016.4.28
 * 由 线性地址 得到 页表项编号
 */
inline u32 get_pte_index(u32 AddrLin)
{
    return ((AddrLin & 0x003FFFFF) >> 12);  // 中间10位A21~A12,0x3FFFFF = 0000 0000 0011 1111 1111 1111 1111 1111
}

/**
 * add by visual 2016.4.28
 * 获取页目录物理地址
 */
inline u32 get_pde_phy_addr(u32 pid)
{
    if (proc_table[pid].task.cr3 == 0) {  // 还没有初始化页目录
        return -1;
    } else {
        return ((proc_table[pid].task.cr3) & 0xFFFFF000);
    }
}

/**
 * add by visual 2016.4.28
 * 获取该线性地址所属页表的物理地址
 */
inline u32 get_pte_phy_addr(
    u32 pid,     // 页目录物理地址
    u32 AddrLin  // 线性地址
)
{
    u32 PageDirPhyAddr = get_pde_phy_addr(pid);
    // 先找到该进程页目录首地址，然后计算出该线性地址对应的页目录项，再访问,最后注意4k对齐
    return (*((u32 *)K_PHY2LIN(PageDirPhyAddr) + get_pde_index(AddrLin))) & 0xFFFFF000;
}

/**
 * add by visual 2016.5.9
 * 获取该线性地址对应的物理页物理地址
 */
inline u32 get_page_phy_addr(
    u32 pid,     // 页表物理地址
    u32 AddrLin  // 线性地址
)
{
    u32 PageTblPhyAddr = get_pte_phy_addr(pid, AddrLin);
    return (*((u32 *)K_PHY2LIN(PageTblPhyAddr) + get_pte_index(AddrLin))) & 0xFFFFF000;
}

/**
 * add by visual 2016.4.28
 * 判断有没有页表
 */
u32 pte_exist(
    u32 PageDirPhyAddr,  // 页目录物理地址
    u32 AddrLin          // 线性地址
)
{
    // 先找到该进程页目录,然后计算出该线性地址对应的页目录项,访问并判断其是否存在
    // 标志位为0，不存在
    if ((0x00000001 & (*((u32 *)K_PHY2LIN(PageDirPhyAddr) + get_pde_index(AddrLin)))) == 0)
        return 0;
    else
        return 1;
}

/**
 * add by visual 2016.4.28
 * 判断该线性地址有没有对应的物理页
 */
u32 phy_exist(
    u32 PageTblPhyAddr,  // 页表物理地址
    u32 AddrLin          // 线性地址
)
{
    // 标志位为 0，不存在
    if ((0x00000001 & (*((u32 *)K_PHY2LIN(PageTblPhyAddr) + get_pte_index(AddrLin)))) == 0)
        return 0;
    else
        return 1;
}

/**
 * add by visual 2016.4.28
 */
void write_page_pde(
    u32 PageDirPhyAddr,  // 页目录物理地址
    u32 AddrLin,         // 线性地址
    u32 TblPhyAddr,      // 要填写的页表的物理地址（函数会进行4k对齐）
    u32 Attribute        // 属性
)
{
    *((u32 *)K_PHY2LIN(PageDirPhyAddr) + get_pde_index(AddrLin)) = (TblPhyAddr & 0xFFFFF000) | Attribute;
    return;
}

/**
 * add by visual 2016.4.28
 */
void write_page_pte(
    u32 TblPhyAddr,  // 页表物理地址
    u32 AddrLin,     // 线性地址
    u32 PhyAddr,     // 要填写的物理页物理地址(任意的物理地址，函数会进行4k对齐)
    u32 Attribute    // 属性
)
{
    *((u32 *)K_PHY2LIN(TblPhyAddr) + get_pte_index(AddrLin)) = (PhyAddr & 0xFFFFF000) | Attribute;
    return;
}

/**
 * add by visual 2016.5.4
 * 从堆中分配size大小的内存，返回线性地址
 */
u32 vmalloc(u32 size)
{
    u32 temp;
    if (p_proc_current->task.info.type == TYPE_PROCESS) {  // 进程直接就是标识
        temp = p_proc_current->task.memmap.heap_lin_limit;
        p_proc_current->task.memmap.heap_lin_limit += size;
    } else {  // 线程需要取父进程的标识
        temp = *((u32 *)p_proc_current->task.memmap.heap_lin_limit);
        (*((u32 *)p_proc_current->task.memmap.heap_lin_limit)) += size;
    }

    return temp;
}

/**
 * add by visual 2016.5.9
 * 将线性地址映射到物理地址上去
 * 函数内部会分配物理地址
 */
int lin_mapping_phy(
    u32 AddrLin,        // 线性地址
    u32 phy_addr,       // 物理地址, 若为 MAX_UNSIGNED_INT(0xFFFFFFFF)，则表示需要由该函数判断是否分配物理地址，否则将 phy_addr 直接和 AddrLin 建立映射
    u32 pid,            // 进程 pid  // edit by visual 2016.5.19
    u32 pde_Attribute,  // 页目录中的属性位
    u32 pte_Attribute   // 页表中的属性位
)
{
    u32 pte_addr_phy;
    u32 pde_addr_phy = get_pde_phy_addr(pid);  // add by visual 2016.5.19

    if (0 == pte_exist(pde_addr_phy, AddrLin)) {             // 页表不存在，创建一个，并填进页目录中
        pte_addr_phy = (u32)do_kmalloc_4k();                 // 为页表申请一页
        memset((void *)K_PHY2LIN(pte_addr_phy), 0, num_4K);  // add by visual 2016.5.26

        if (pte_addr_phy < 0 || (pte_addr_phy & 0x3FF) != 0)  // add by visual 2016.5.9
        {
            kern_set_color(MAKE_COLOR(GREY, RED));
            kern_display_string("lin_mapping_phy Error:pte_addr_phy");
            kern_set_color(WHITE);
            return -1;
        }

        write_page_pde(
            pde_addr_phy,  // 页目录物理地址
            AddrLin,       // 线性地址
            pte_addr_phy,  // 页表物理地址
            pde_Attribute  // 属性
        );
    } else {  // 页表存在，获取该页表物理地址
        pte_addr_phy = get_pte_phy_addr(
            pid,     // 进程 pid  // edit by visual 2016.5.19
            AddrLin  // 线性地址
        );
    }

    if (MAX_UNSIGNED_INT == phy_addr)                 // add by visual 2016.5.19
    {                                                 // 由函数申请内存
        if (0 == phy_exist(pte_addr_phy, AddrLin)) {  // 无物理页，申请物理页并修改phy_addr
            if (AddrLin >= K_PHY2LIN(0))
                phy_addr = do_kmalloc_4k();  // 从内核物理地址申请一页
            else {
                // kprintf("%");
                phy_addr = do_malloc_4k();  // 从用户物理地址空间申请一页
            }
        } else {
            // 有物理页，什么也不做,直接返回，必须返回
            return 0;
        }
    } else {  // 指定填写phy_addr
              // 不用修改phy_addr
    }

    if (phy_addr < 0 || (phy_addr & 0x3FF) != 0) {
        kern_set_color(MAKE_COLOR(GREY, RED));
        kern_set_color(WHITE);
        ("lin_mapping_phy:phy_addr ERROR");
        kern_set_color(WHITE);
        return -1;
    }

    write_page_pte(
        pte_addr_phy,  // 页表物理地址
        AddrLin,       // 线性地址
        phy_addr,      // 物理页物理地址
        pte_Attribute  // 属性
    );
    refresh_page_cache();

    return 0;
}

/**
 * add by visual 2016.5.12
 * 将内核低端页表清除
 */
void clear_kernel_pagepte_low()
{
    u32 page_num = *(u32 *)PageTblNumAddr;
    memset((void *)(K_PHY2LIN(KernelPageTblAddr)), 0, 4 * page_num);              // 从内核页目录中清除内核页目录项前8项
    memset((void *)(K_PHY2LIN(KernelPageTblAddr + 0x1000)), 0, 4096 * page_num);  // 从内核页表中清除线性地址的低端映射关系
    refresh_page_cache();
}

/*
 * MINIOS 中比较通用的页表映射函数
 * 它将 laddr 处的虚拟页面映射到物理地址为 paddr（如果 paddr 为 -1 则会自动申请一个新的物理页面）的物理页面
 * 并将 pte_flag 置位到页表项（页目录项标志位默认为 PTE_P | PTE_W | PTE_U）
 * 这个函数中所有新申请到的页面信息会存放到 page_list 这个链表中
 */
static void lin_mapping_phy_boot(u32 cr3, uintptr_t laddr, phyaddr_t paddr, u32 pte_flag)
{
    assert(PGOFF(laddr) == 0);

    uintptr_t *pde_ptr = (uintptr_t *)K_PHY2LIN(cr3);

    if ((pde_ptr[PDX(laddr)] & PG_P) == 0) {
        phyaddr_t pte_phy = do_kmalloc_4k();
        memset((void *)K_PHY2LIN(pte_phy), 0, PGSIZE);
        pde_ptr[PDX(laddr)] = pte_phy | PG_P | PG_RWW | PG_USU;
    }

    phyaddr_t  pte_phy = PTE_ADDR(pde_ptr[PDX(laddr)]);
    uintptr_t *pte_ptr = (uintptr_t *)K_PHY2LIN(pte_phy);

    phyaddr_t page_phy;
    if (paddr == (phyaddr_t)-1) {
        if ((pte_ptr[PTX(laddr)] & PG_P) != 0)
            return;
        page_phy = do_kmalloc_4k();
    } else {
        if ((pte_ptr[PTX(laddr)] & PG_P) != 0)
            warn("this page was mapped before, laddr: %x", laddr);
        assert(PGOFF(paddr) == 0);
        page_phy = paddr;
    }
    pte_ptr[PTX(laddr)] = page_phy | pte_flag;
}

/*
 * 初始化进程页表的内核部分
 * 将 3GB ~ 3GB + 128MB 的线性地址映射到 0 ~ 128MB 的物理地址
 */
void map_kern(u32 cr3)
{
    for (phyaddr_t paddr = 0; paddr < KernelSize; paddr += PGSIZE) {
        lin_mapping_phy_boot(cr3, K_PHY2LIN(paddr), paddr, PG_P | PG_RWW | PG_USU);
    }
}

/**
 * 找到 pgdir 中对应虚拟地址的页表项，返回其虚拟地址
 */
pte_t *pgdir_walk(pde_t *pgdir, const void *va, const int create)
{
    uint32_t pdx = PDX((uint32_t)va);
    uint32_t ptx = PTX((uint32_t)va);
    pde_t   *pde;
    pte_t   *pte;

    pde = pgdir + pdx;

    // kprintf("%x ", *pde);

    if (*pde & PG_P) {
        // 如果页目录项存在
        pte = (pte_t *)K_PHY2LIN(*pde & ~0xFFF);
    } else {
        // 页目录项不存在
        if (!create) {
            return NULL;
        } else {
            // 为页目录申请一页
            uint32_t pte_phy = do_kmalloc_4k();
            if (!pte_phy) {
                // 分配失败
                return NULL;
            }
            *pde = pte_phy | (PG_P | PG_RWW | PG_USU);
            // 获取页表
            pte = (pte_t *)K_PHY2LIN(*pde & ~0xFFF);
            // 测试一下
            // kprintf("%x ", *pte);
            // kprintf("%x ", *(pte + ptx));
        }
    }
    // 返回页表项的虚拟地址
    return pte + ptx;
}

/**
 * 将 pgdir 中的 [va, va+size) 虚拟地址映射到 [pa, pa+size) 物理地址
 */
static void boot_map_region(pde_t *pgdir, uintptr_t va, size_t size, uint32_t pa, int perm)
{
    // 计算页数
    size_t pgs = size / PGSIZE;
    if (size % PGSIZE != 0) {
        pgs++;
    }
    // 逐页映射
    for (int i = 0; i < pgs; i++) {
        pte_t *pte = pgdir_walk(pgdir, (void *)va, 1);
        if (pte == NULL) {
            panic("boot_map_region out of memory\n");
        }
        *pte = pa | PG_P | perm;  // ERROR HERE
        va += PGSIZE;
        pa += PGSIZE;
    }
}

// 保存当前 MMIO 分配的最高地址，随着每次分配都会增长
static uintptr_t mmio_base = MMIOBASE;

/**
 * MMIO 区域为 [MMIOBASE, MMIOLIM]
 * 从 MMIO 区域中映射 size bytes 到 [pa, pa+size)
 * 返回映射区域的基地址 base
 * size 可以不是 PGSIZE 的整数倍
 */
uint32_t *mmio_map_region(uint32_t pa, uint32_t size)
{
    // 1. 从 MMIO 区域中预留出 size 大小的地址
    void *ret = (void *)mmio_base;
    size = ROUNDUP(size, PGSIZE);
    if (mmio_base + size > MMIOLIM || mmio_base + size < mmio_base) {
        panic("mmio_map_region reservation overflow\n");
    }
    // 2. 将 [base, base+size) 线性地址映射到 [pa, pa+size)
    uint32_t kern_cr3;
    __asm__ __volatile__(
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %0\n\t"
        : "=m"(kern_cr3)
        : /* no input */
        : "%eax"
    );
    pde_t *pde_addr_phy = (pde_t *)(K_PHY2LIN(kern_cr3 & 0xFFFFF000));
    boot_map_region(pde_addr_phy, mmio_base, size, pa, PG_RWW | PG_PCD | PG_PWT);
    return ret;
}

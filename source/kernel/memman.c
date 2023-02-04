#include <kernel/memman.h>

#include <kernlib/stdio.h>
#include <kernlib/string.h>

u32 MemInfo[256] = {0};  // 存放 FMIBuff 后 1k 内容

struct MEMMAN  s_memman;
struct MEMMAN *memman = &s_memman;  // (struct MEMMAN *) MEMMAN_ADDR;


void memman_init(struct MEMMAN *man);
u32  memman_alloc(struct MEMMAN *man, u32 size);
u32  memman_kalloc(struct MEMMAN *man, u32 size);
u32  memman_alloc_4k(struct MEMMAN *man);
u32  memman_kalloc_4k(struct MEMMAN *man);
u32  memman_free(struct MEMMAN *man, u32 addr, u32 size);
void disp_free();
u32  memman_total(struct MEMMAN *man);

// 于 kernel_main() 中调用，进行初始化
// 将用到的物理空间全部索引起来
void init()
{
    u32 memstart = MEMSTART;  // 4M 开始初始化
    u32 i, j;

    memcpy(MemInfo, (u32 *)FMIBuff, 1024);  // 复制内存

    memman_init(memman);  // 初始化 memman 中 frees, maxfrees, lostsize, losts

    for (i = 1; i <= MemInfo[0]; i++) {
        if (MemInfo[i] < memstart) continue;                   // 4M 之后开始 free
        memman_free(memman, memstart, MemInfo[i] - memstart);  // free 每一段可用内存
        memstart = MemInfo[i] + 0x1000;                        // memtest_sub(start, end) 中每 4KB 检测一次
    }

    // 6M 处分开，4～6M 为 kmalloc_4k 使用，6～8M 为 kmalloc 使用
    for (i = 0; i < memman->frees; i++) {
        if ((memman->free[i].addr <= KWALL) && (memman->free[i].addr + memman->free[i].size > KWALL)) {
            if (memman->free[i].addr == KWALL)
                break;
            else {
                for (j = memman->frees; j > i + 1; j--) {  // i 之后向后一位
                    memman->free[j] = memman->free[j - 1];
                }
                memman->frees++;
                if (memman->maxfrees < memman->frees) {  // 更新 man->maxfrees
                    memman->maxfrees = memman->frees;
                }
                memman->free[i + 1].addr = KWALL;
                memman->free[i + 1].size = memman->free[i].addr + memman->free[i].size - KWALL;
                memman->free[i].size = KWALL - 0x1000 - memman->free[i].addr;
                break;
            }
        }
    }

    // 8M 处分开，4～8M 为 kmalloc 使用，8～32M 为 malloc 使用
    for (i = 0; i < memman->frees; i++) {
        if ((memman->free[i].addr <= WALL) && (memman->free[i].addr + memman->free[i].size > WALL)) {
            if (memman->free[i].addr == WALL)
                break;
            else {
                for (j = memman->frees; j > i + 1; j--) {
                    memman->free[j] = memman->free[j - 1];
                }
                memman->frees++;
                if (memman->maxfrees < memman->frees) {
                    memman->maxfrees = memman->frees;
                }
                memman->free[i + 1].addr = WALL;
                memman->free[i + 1].size = memman->free[i].addr + memman->free[i].size - WALL;
                memman->free[i].size = WALL - 0x1000 - memman->free[i].addr;
                break;
            }
        }
    }

    // 16M 处分开，8～16M 为 malloc 使用，16～32M 为 malloc_4k 使用
    for (i = 0; i < memman->frees; i++) {
        if ((memman->free[i].addr <= UWALL) && (memman->free[i].addr + memman->free[i].size > UWALL)) {
            if (memman->free[i].addr == UWALL)
                break;
            else {
                for (j = memman->frees; j > i + 1; j--) {
                    memman->free[j] = memman->free[j - 1];
                }
                memman->frees++;
                if (memman->maxfrees < memman->frees) {
                    memman->maxfrees = memman->frees;
                }
                memman->free[i + 1].addr = UWALL;
                memman->free[i + 1].size = memman->free[i].addr + memman->free[i].size - UWALL;
                memman->free[i].size = UWALL - 0x1000 - memman->free[i].addr;
                break;
            }
        }
    }
    kprintf("Memory Available: %d\n", memman_total(memman));
    return;
}

void memman_init(struct MEMMAN *man)
{
    man->frees = 0;
    man->maxfrees = 0;
    man->lostsize = 0;
    man->losts = 0;
    return;
}

u32 memman_kalloc_4k(struct MEMMAN *man)
{
    u32 i, a;
    u32 size = 0x1000;
    for (i = 0; i < man->frees; i++) {
        if ((man->free[i].addr >= MEMSTART) && (man->free[i].addr + size < KWALL)) {
            if (man->free[i].size >= size) {
                a = man->free[i].addr;
                man->free[i].addr += size;
                man->free[i].size -= size;
                if (man->free[i].size == 0) {
                    man->frees--;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i + 1];
                    }
                }
                return a;
            }
        }
    }
    return 0;
}

u32 memman_kalloc(struct MEMMAN *man, u32 size)
{
    u32 i, a;
    for (i = 0; i < man->frees; i++) {
        if ((man->free[i].addr >= KWALL) && (man->free[i].addr + size < WALL)) {
            if (man->free[i].size >= size) {
                a = man->free[i].addr;
                man->free[i].addr += size;
                man->free[i].size -= size;
                if (man->free[i].size == 0) {
                    man->frees--;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i + 1];
                    }
                }
                return a;
            }
        }
    }
    return -1;
}

u32 memman_alloc(struct MEMMAN *man, u32 size)
{
    u32 i, a;
    for (i = 0; i < man->frees; i++) {
        if ((man->free[i].addr >= WALL) && (man->free[i].addr + size < UWALL)) {
            if (man->free[i].size >= size) {
                a = man->free[i].addr;
                man->free[i].addr += size;
                man->free[i].size -= size;
                if (man->free[i].size == 0) {
                    man->frees--;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i + 1];
                    }
                }
                return a;
            }
        }
    }
    return -1;
}

u32 memman_alloc_4k(struct MEMMAN *man)
{
    u32 i, a;
    u32 size = 0x1000;
    for (i = 0; i < man->frees; i++) {
        if ((man->free[i].addr >= UWALL) && (man->free[i].addr + size < MEMEND)) {
            if (man->free[i].size >= size) {
                a = man->free[i].addr;
                man->free[i].addr += size;
                man->free[i].size -= size;
                if (man->free[i].size == 0) {
                    man->frees--;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i + 1];
                    }
                }
                return a;
            }
        }
    }
    return -1;
}

u32 memman_free(struct MEMMAN *man, u32 addr, u32 size)
{
    int i, j;

    if (size == 0) return 0;  // 初始化时，防止有连续坏块

    for (i = 0; i < man->frees; i++) {
        if (man->free[i].addr > addr) {
            break;
        }
    }  // man->free[i-1].addr < addr <man->free[i].addr

    if (i > 0) {                                                      // 前面有可分配内存
        if (man->free[i - 1].addr + man->free[i - 1].size == addr) {  // 与前面相邻
            man->free[i - 1].size += size;
            if (i < man->frees) {                        // 后面有可分配内存
                if (addr + size == man->free[i].addr) {  // 同时与后面相邻
                    man->free[i - 1].size += man->free[i].size;
                    man->frees--;
                    for (; i < man->frees; i++) {
                        man->free[i] = man->free[i + 1];  // 结构体赋值，i之后向前一位
                    }
                }
            }
            return 0;
        }  // 与前面不相邻
    }      // 前面无可分配内存

    if (i < man->frees) {                        // 后面有可分配内存
        if (addr + size == man->free[i].addr) {  // 与后面相邻
            man->free[i].addr = addr;
            man->free[i].size += size;
            return 0;
        }
    }

    if (man->frees < MEMMAN_FREES) {        // 数组未满
        for (j = man->frees; j > i; j--) {  // i 之后向后一位
            man->free[j] = man->free[j - 1];
        }
        man->frees++;
        if (man->maxfrees < man->frees) {  // 更新man->maxfrees
            man->maxfrees = man->frees;
        }
        man->free[i].addr = addr;  // 插入
        man->free[i].size = size;
        return 0;
    }

    man->losts++;  // free失败
    man->lostsize += size;
    return -1;
}

u32 memman_free_4k(struct MEMMAN *man, u32 addr)
{
    return memman_free(man, addr, 0x1000);
}


u32 do_malloc(u32 size)
{
    return memman_alloc(memman, size);
}

u32 do_kmalloc(u32 size)
{
    return memman_kalloc(memman, size);
}

u32 do_malloc_4k()
{
    return memman_alloc_4k(memman);
}

u32 do_kmalloc_4k()
{
    return memman_kalloc_4k(memman);
}

u32 do_free(u32 addr, u32 size)
{
    return memman_free(memman, addr, size);
}

u32 do_free_4k(u32 addr)
{
    return memman_free_4k(memman, addr);
}

void disp_free()
{
    for (int i = 0; i < memman->frees; i++)
        kprintf("0x%x#0x%x###", memman->free[i].addr, memman->free[i].size);
}

// free 总容量
u32 memman_total(struct MEMMAN *man)
{  
    u32 i, t = 0;
    for (i = 0; i < man->frees; i++) {
        t += man->free[i].size;
    }
    return t;
}

void memman_test()
{
    u32 *p1 = 0;
    u32 *p2 = 0;
    u32 *p3 = 0;
    u32 *p4 = 0;
    u32 *p = 0;

    const uint32_t TEST = 0x11223344;

    p1 = (u32 *)do_malloc(4);
    if (-1 != (u32)p1) {  // 打印 p1，当前空闲内存信息，p1 所指内存的内容
        kprintf("START");
        kern_display_integer((u32)p1);
        // disp_free();
        *p1 = TEST;
        kern_display_integer(*p1);
        kprintf("END");
    }

    p2 = (u32 *)do_kmalloc(4);
    if (-1 != (u32)p2) {
        kprintf("START");
        kern_display_integer((u32)p2);
        // disp_free();
        *p2 = TEST;
        kern_display_integer(*p2);
        kprintf("END");
    }

    do_free((u32)p1, 4);
    do_free((u32)p2, 4);

    p3 = (u32 *)do_malloc_4k();
    if (-1 != (u32)p3) {
        kprintf("START");
        kern_display_integer((u32)p3);
        // disp_free();
        *p3 = TEST;
        kern_display_integer(*p3);
        p = p3 + 2044;
        *p = 0x22334455;
        kern_display_integer(*p);
        p += 2048;
        *p = 0x33445566;
        kern_display_integer(*p);
        kprintf("END");
    }

    p4 = (u32 *)do_kmalloc_4k(4);
    if (-1 != (u32)p4) {
        kprintf("START");
        kern_display_integer((u32)p4);
        // disp_free();
        *p4 = TEST;
        kern_display_integer(*p4);
        p = p4 + 2044;
        *p = 0x22334455;
        kern_display_integer(*p);
        p += 2048;
        *p = 0x33445566;
        kern_display_integer(*p);
        kprintf("END");
    }

    do_free_4k((u32)p3);
    do_free_4k((u32)p4);

    kprintf("START");
    disp_free();
    kprintf("END");

    return;
}
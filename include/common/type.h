/**
 * type.h
 */

#ifndef _ORANGES_TYPE_H_
#define _ORANGES_TYPE_H_

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef _Bool bool;
enum { false,
       true };

#define TRUE  1
#define FALSE 0

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define MAX_UNSIGNED_INT 0xFFFFFFFF  // 最大的无符号整形
#define MAX_INT          0x7FFFFFFF  // 最大的整形数

#define B  (1LLU)
#define KB (1024LLU * B)
#define MB (1024LLU * KB)
#define GB (1024LLU * MB)

typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef short              int16_t;
typedef unsigned short     uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
typedef long long          int64_t;
typedef unsigned long long uint64_t;

typedef char               i8;
typedef unsigned char      u8;
typedef short              i16;
typedef unsigned short     u16;
typedef int                i32;
typedef unsigned int       u32;
typedef long long          i64;
typedef unsigned long long u64;

typedef i32 intptr_t;
typedef u32 uintptr_t;

// 通常描述一个对象的大小，会根据机器的型号变化类型
typedef u32 size_t;
// signed size_t 通常描述系统调用返回值，会根据机器的型号变化类型
typedef i32 ssize_t;
// 通常描述偏移量，会根据机器的型号变化类型
typedef i32 off_t;
// 通常描述物理地址
typedef u32 phyaddr_t;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static inline uint16_t bswaps(uint16_t val)
{
    return (
        ((val & 0x00ffU) << 8) |
        ((val & 0xff00U) >> 8)
    );
}

static inline uint32_t bswapl(uint32_t val)
{
    return (
        ((val & 0x000000ffUL) << 24) |
        ((val & 0x0000ff00UL) << 8) |
        ((val & 0x00ff0000UL) >> 8) |
        ((val & 0xff000000UL) >> 24)
    );
}

#define ntohs bswaps
#define ntohl bswapl
#define htons bswaps
#define htonl bswapl

typedef uint32_t pte_t;
typedef uint32_t pde_t;

// Rounding operations (efficient when n is a power of 2)
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n) \
    ({ \
        uint32_t __a = (uint32_t)(a); \
        (typeof(a))(__a - __a % (n)); \
    })
// Round up to the nearest multiple of n
#define ROUNDUP(a, n) \
    ({ \
        uint32_t __n = (uint32_t)(n); \
        (typeof(a))(ROUNDDOWN((uint32_t)(a) + __n - 1, __n)); \
    })

// page directory index
#define PDX(la) ((((uintptr_t)(la)) >> PDXSHIFT) & 0x3FF)

// page table index
#define PTX(la) ((((uintptr_t)(la)) >> PTXSHIFT) & 0x3FF)

// offset in page
#define PGOFF(la) (((uintptr_t)(la)) & 0xFFF)

// Address in page table or page directory entry
#define PTE_ADDR(pte) ((phyaddr_t)(pte) & ~0xFFF)

#define __ALIGN_KERNEL_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define __ALIGN_KERNEL(x, a)         __ALIGN_KERNEL_MASK(x, (typeof(x))(a)-1)

#define ALIGN(x, a)           __ALIGN_KERNEL((x), (a))
#define ALIGN_DOWN(x, a)      __ALIGN_KERNEL((x) - ((a)-1), (a))
#define __ALIGN_MASK(x, mask) __ALIGN_KERNEL_MASK((x), (mask))
#define PTR_ALIGN(p, a)       ((typeof(p))ALIGN((unsigned long)(p), (a)))
#define PTR_ALIGN_DOWN(p, a)  ((typeof(p))ALIGN_DOWN((unsigned long)(p), (a)))
#define IS_ALIGNED(x, a)      (((x) & ((typeof(x))(a)-1)) == 0)

#endif /* _ORANGES_TYPE_H_ */

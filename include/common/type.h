/**
 * type.h
 *
 * Forrest Yu
 * 2005
 */

#ifndef _ORANGES_TYPE_H_
#define _ORANGES_TYPE_H_

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef _Bool bool;
enum {
    false,
    true
};

/* Boolean */
#define TRUE  1
#define FALSE 0

/* max() & min() */
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

/* 最大整数定义 */
#define MAX_UNSIGNED_INT 0xFFFFFFFF  // 最大的无符号整形
#define MAX_INT          0x7FFFFFFF  // 最大的整形数

/* added by PCI: Start */

// Explicitly-sized versions of integer types
typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef short              int16_t;
typedef unsigned short     uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
typedef long long          int64_t;
typedef unsigned long long uint64_t;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/* added by PCI: End */

typedef long long          i64;
typedef unsigned long long u64;
typedef int                i32;
typedef unsigned int       u32;
typedef short              i16;
typedef unsigned short     u16;
typedef char               i8;
typedef unsigned char      u8;

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

static inline uint16_t bswaps(uint16_t val)
{
  return (((val & 0x00ffU) << 8) |
          ((val & 0xff00U) >> 8));
}

static inline uint32_t bswapl(uint32_t val)
{
  return (((val & 0x000000ffUL) << 24) |
          ((val & 0x0000ff00UL) << 8) |
          ((val & 0x00ff0000UL) >> 8) |
          ((val & 0xff000000UL) >> 24));
}

#define ntohs bswaps
#define ntohl bswapl
#define htons bswaps
#define htonl bswapl

#endif /* _ORANGES_TYPE_H_ */

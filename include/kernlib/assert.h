#ifndef _MINIOS_ASSERT_H_
#define _MINIOS_ASSERT_H_

void _warn(const char *, int, const char *, ...);
void _panic(const char *, int, const char *, ...) __attribute__((__noreturn__));

#define warn(...) _warn(__FILE__, __LINE__, __VA_ARGS__)
#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

#define assert(x) \
    do { if (!(x)) panic("assertion failed: %s", #x); } while (false)

// 静态 assert，如果不符合条件就会直接在编译期报错
#define static_assert(x) switch (x) case 0: case (x):;

#endif /* _MINIOS_ASSERT_H_ */
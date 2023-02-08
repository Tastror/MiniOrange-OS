#include <kernlib/string.h>

int strlen(const char *s)
{
    int n;

    for (n = 0; *s != '\0'; s++)
        n++;
    return n;
}

int strnlen(const char *s, size_t size)
{
    int n;

    for (n = 0; size > 0 && *s != '\0'; s++, size--)
        n++;
    return n;
}

char *strcpy(char *dst, const char *src)
{
    char *ret;

    ret = dst;
    while ((*dst++ = *src++) != '\0')
        /* do nothing */;
    return ret;
}

char *strcat(char *dst, const char *src)
{
    int len = strlen(dst);
    strcpy(dst + len, src);
    return dst;
}

char *strncpy(char *dst, const char *src, size_t size)
{
    size_t i;
    char  *ret;

    ret = dst;
    for (i = 0; i < size; i++) {
        *dst++ = *src;
        // If strlen(src) < size, null-pad 'dst' out to 'size' chars
        if (*src != '\0')
            src++;
    }
    return ret;
}

int strcmp(const char *p, const char *q)
{
    while (*p && *p == *q)
        p++, q++;
    return (int)((unsigned char)*p - (unsigned char)*q);
}

int strncmp(const char *p, const char *q, size_t n)
{
    while (n > 0 && *p && *p == *q)
        n--, p++, q++;
    if (n == 0)
        return 0;
    else
        return (int)((unsigned char)*p - (unsigned char)*q);
}

size_t strspn(const char *str, const char *accept)
{
    if (accept[0] == '\0')
        return 0;
    if ((accept[1] == '\0')) {
        const char *a = str;
        for (; *str == *accept; str++)
            ;
        return str - a;
    }
    unsigned char  table[256];
    unsigned char *p = memset(table, 0, 64);
    memset(p + 64, 0, 64);
    memset(p + 128, 0, 64);
    memset(p + 192, 0, 64);
    unsigned char *s = (unsigned char *)accept;
    do
        p[*s++] = 1;
    while (*s);
    s = (unsigned char *)str;
    if (!p[s[0]]) return 0;
    if (!p[s[1]]) return 1;
    if (!p[s[2]]) return 2;
    if (!p[s[3]]) return 3;
    s = (unsigned char *)PTR_ALIGN_DOWN(s, 4);
    unsigned int c0, c1, c2, c3;
    do {
        s += 4;
        c0 = p[s[0]];
        c1 = p[s[1]];
        c2 = p[s[2]];
        c3 = p[s[3]];
    } while ((c0 & c1 & c2 & c3) != 0);
    size_t count = s - (unsigned char *)str;
    return (c0 & c1) == 0 ? count + c0 : count + c2 + 2;
}

char *strchr(register const char *s, int c)
{
    do {
        if (*s == c) {
            return (char *)s;
        }
    } while (*s++);
    return (0);
}

char *strchrnul(const char *s, int c_in)
{
    const unsigned char     *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int        longword, magic_bits, charmask;
    unsigned char            c;
    c = (unsigned char)c_in;
    for (char_ptr = (const unsigned char *)s;
         ((unsigned long int)char_ptr & (sizeof(longword) - 1)) != 0;
         ++char_ptr)
        if (*char_ptr == c || *char_ptr == '\0')
            return (void *)char_ptr;
    longword_ptr = (unsigned long int *)char_ptr;
    magic_bits = -1;
    magic_bits = magic_bits / 0xff * 0xfe << 1 >> 1 | 1;
    charmask = c | (c << 8);
    charmask |= charmask << 16;
    if (sizeof(longword) > 4)
        charmask |= (charmask << 16) << 16;
    // if (sizeof(longword) > 8)
    //     abort();
    for (;;) {
        longword = *longword_ptr++;
        if ((((longword + magic_bits) ^ ~longword) & ~magic_bits) != 0 || ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask)) & ~magic_bits) != 0) {
            const unsigned char *cp = (const unsigned char *)(longword_ptr - 1);
            if (*cp == c || *cp == '\0')
                return (char *)cp;
            if (*++cp == c || *cp == '\0')
                return (char *)cp;
            if (*++cp == c || *cp == '\0')
                return (char *)cp;
            if (*++cp == c || *cp == '\0')
                return (char *)cp;
            if (sizeof(longword) > 4) {
                if (*++cp == c || *cp == '\0')
                    return (char *)cp;
                if (*++cp == c || *cp == '\0')
                    return (char *)cp;
                if (*++cp == c || *cp == '\0')
                    return (char *)cp;
                if (*++cp == c || *cp == '\0')
                    return (char *)cp;
            }
        }
    }
    /* This should never happen.  */
    return NULL;
}

size_t strcspn(const char *str, const char *reject)
{
    if ((reject[0] == '\0') || (reject[1] == '\0'))
        return strchrnul(str, reject[0]) - str;
    unsigned char  table[256];
    unsigned char *p = memset(table, 0, 64);
    memset(p + 64, 0, 64);
    memset(p + 128, 0, 64);
    memset(p + 192, 0, 64);
    unsigned char *s = (unsigned char *)reject;
    unsigned char  tmp;
    do
        p[tmp = *s++] = 1;
    while (tmp);
    s = (unsigned char *)str;
    if (p[s[0]]) return 0;
    if (p[s[1]]) return 1;
    if (p[s[2]]) return 2;
    if (p[s[3]]) return 3;
    s = (unsigned char *)PTR_ALIGN_DOWN(s, 4);
    unsigned int c0, c1, c2, c3;
    do {
        s += 4;
        c0 = p[s[0]];
        c1 = p[s[1]];
        c2 = p[s[2]];
        c3 = p[s[3]];
    } while ((c0 | c1 | c2 | c3) == 0);
    size_t count = s - (unsigned char *)str;
    return (c0 | c1) != 0 ? count - c0 + 1 : count - c2 + 3;
}

char *strtok(char *s, const char *delim)
{
    static char *lasts;
    register int ch;

    if (s == 0)
        s = lasts;
    do {
        if ((ch = *s++) == '\0')
            return 0;
    } while (strchr(delim, ch));
    --s;
    lasts = s + strcspn(s, delim);
    if (*lasts != 0)
        *lasts++ = 0;
    return s;
}

void *memset(void *v, int c, size_t n)
{
    char *p;
    int   m;

    p = v;
    m = n;
    while (--m >= 0)
        *p++ = c;

    return v;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    const char *s;
    char       *d;

    s = src;
    d = dst;

    if (s < d && s + n > d) {
        s += n;
        d += n;
        while (n-- > 0)
            *--d = *--s;
    } else {
        while (n-- > 0)
            *d++ = *s++;
    }

    return dst;
}

void *memmove(void *dst, const void *src, uint32_t n)
{
    const char *s;
    char       *d;

    if (n == 0)
        return dst;

    s = src;
    d = dst;
    if (s < d && s + n > d) {
        s += n;
        d += n;
        while (n-- > 0)
            *--d = *--s;
    } else
        while (n-- > 0)
            *d++ = *s++;

    return dst;
}

char *itoa(char *str, int num) /* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
{
    char *p = str;
    char  ch;
    int   i;
    int   flag = FALSE;

    *p++ = '0';
    *p++ = 'x';

    if (num == 0) {
        *p++ = '0';
    } else {
        for (i = 28; i >= 0; i -= 4) {
            ch = (num >> i) & 0xF;
            if (flag || (ch > 0)) {
                flag = TRUE;
                ch += '0';
                if (ch > '9') {
                    ch += 7;
                }
                *p++ = ch;
            }
        }
    }

    *p = 0;

    return str;
}
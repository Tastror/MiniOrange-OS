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

void*
memmove(void *dst, const void *src, uint32_t n)
{
  const char *s;
  char *d;

  if(n == 0)
    return dst;
  
  s = src;
  d = dst;
  if(s < d && s + n > d){
    s += n;
    d += n;
    while(n-- > 0)
      *--d = *--s;
  } else
    while(n-- > 0)
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
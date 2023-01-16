#ifndef _MINIOS_STRING_H_
#define _MINIOS_STRING_H_

#include <common/type.h>
#include <define/define.h>

int   strlen(const char *s);
int   strnlen(const char *s, size_t size);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t size);
char *strcat(char *dst, const char *src);
int   strcmp(const char *s1, const char *s2);
int   strncmp(const char *s1, const char *s2, size_t size);
long  strtol(const char *cp, char **endp, unsigned int base);
char *itoa(char *str, int num);
void *memset(void *v, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
void* memmove(void *dst, const void *src, uint32_t n);

#endif /* _MINIOS_STRING_H_ */
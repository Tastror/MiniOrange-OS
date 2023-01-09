// to show what is in a big file

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef char               i1;
typedef unsigned char      u1;
typedef short              i2;
typedef unsigned short     u2;
typedef int                i4;
typedef unsigned int       u4;
typedef long long          i8;
typedef unsigned long long u8;

#define B  (1LLU)
#define KB (1024LLU * B)
#define MB (1024LLU * KB)
#define GB (1024LLU * MB)

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("please input the file name\n");
        return 0;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        printf("open error: %s\n", strerror(errno));
        return 0;
    }
    char buff[32];
    u8 count = 0;
    while (1) {
        int read_res = read(fd, buff, 1);
        if (read_res <= 0) {
            printf("read done: %s\n", strerror(errno));
            return 0;
        }
        u1 num = *(u1*)buff;
        if (num != 0)
            printf("%llu: %u\n", count, num);
        count++;
    }
}
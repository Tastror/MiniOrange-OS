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

u8 position[] = {
    440, 441, 442, 443, 446, 447, 448, 450,
    451, 452, 455, 458, 459, 463, 464, 466,
    467, 468, 469, 471, 474, 475, 476, 510, 511,
    2097599, 2097600, 2097602, 2097603, 2097604,
    2097605, 2097607, 2097610, 2097611, 2097615,
    2097616, 2097617, 2097618, 2097619, 2097620,
    2097621, 2097623, 2097626, 2097627, 2097662, 2097663,
    26214847, 26214848, 26214849, 26214850, 26214851, 26214852,
    26214853, 26214855, 26214858, 26214859, 26214910, 26214911,
    51199999
};
u1 value[] = {
    106, 169, 185, 211, 128, 32, 33, 131,
    63, 32, 8, 161, 7, 65, 2, 5,
    57, 19, 6, 16, 160, 118, 1, 85, 170,
    97, 34, 131, 28, 42,
    3, 8, 81, 171, 47,
    45, 3, 5, 57, 19,
    6, 184, 160, 190, 85, 170,
    80, 14, 3, 131, 57, 19,
    6, 8, 160, 182, 85, 170,
    0
};

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("please input the file name\n");
        return 0;
    }
    int fd = open(argv[1], O_CREAT | O_RDWR, 0777);
    char buff[4];
    u8 len = sizeof(position) / sizeof(u8);
    for (u8 i = 0; i < len; ++i) {
        lseek(fd, position[i], SEEK_SET);
        *(u1*)buff = value[i];
        int write_res = write(fd, buff, 1);
        if (write_res <= 0) {
            printf("write error: %s\n", strerror(errno));
            return 0;
        }
    }
}
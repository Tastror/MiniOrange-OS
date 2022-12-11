#include "const.h"
#include "global.h"
#include "proc.h"
#include "protect.h"
#include "proto.h"
#include "stdio.h"
#include "string.h"
#include "type.h"

int main(int arg, char *argv[])
{
    int stdin = open("dev_tty0", O_RDWR);
    int stdout = open("dev_tty0", O_RDWR);
    int stderr = open("dev_tty0", O_RDWR);

    char buf[1024];
    int  pid;
    int  times = 0;
    while (1) {
        printf("\nminiOS:/ $ ");
        if (gets(buf) && strlen(buf) != 0) {
            if (exec(buf) != 0) {
                printf("exec failed: file not found!\n");
                continue;
            }
        }
    }
}
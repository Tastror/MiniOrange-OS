#include <lib/stdio.h>
#include <lib/string.h>
#include <kernel/syscall.h>

int main(int arg, char *argv[])
{
    int stdin = open("dev_tty0", O_RDWR);
    int stdout = open("dev_tty0", O_RDWR);
    int stderr = open("dev_tty0", O_RDWR);

    char buf[1024];
    int  pid;
    int  times = 0;
    while (true) {
        set_color(0x02);
        printf("miniOS");
        set_color(0x0F);
        printf(" $ ");
        if (gets(buf) && strlen(buf) != 0) {
            if (exec(buf) != 0) {
                set_color(0x04);
                printf("exec failed: file not found!\n");
                set_color(0x0F);
                continue;
            }
        }
    }
}
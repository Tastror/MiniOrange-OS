#include <lib/stdio.h>
#include <lib/string.h>

#include <kernel/syscall.h>
#include <device/pci.h>

int main(int arg, char *argv[])
{
    int stdin = open("dev_tty0", O_RDWR);
    int stdout = open("dev_tty0", O_RDWR);
    int stderr = open("dev_tty0", O_RDWR);

    char buf[1024];
    int  pid;
    int  times = 0;
    while (true) {
        set_color(GREEN);
        printf("MiniOS");
        set_color(BLUE);
        printf(" $ ");
        set_color(WHITE);
        if (gets(buf) && strlen(buf) != 0) {
            if (strcmp(buf, "help") == 0) {
                printf("\n");
                set_color(CYAN);
                printf("HELP MENU\n");
                set_color(WHITE);
                printf("use 'orange/xxx.bin' to execute a user file\n");
                printf("use 'net' to try the net\n");
                printf("\n");
            }
            else if (strcmp(buf, "net") == 0) {
                printf("trying net now...\n");
            }
            else if (exec(buf) != 0) {
                set_color(RED);
                printf("exec failed: file not found!\n");
                set_color(WHITE);
                continue;
            }
        }
    }
}
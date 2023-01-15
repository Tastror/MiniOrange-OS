#include <userlib/fcntl.h>
#include <userlib/stdio.h>
#include <userlib/stdlib.h>
#include <userlib/string.h>

int main(int arg, char *argv[])
{
    int stdin = open("dev_tty0", O_RDWR);
    int stdout = open("dev_tty0", O_RDWR);
    int stderr = open("dev_tty0", O_RDWR);

    char buf[1024];
    int  pid;
    int  times = 0;

    printf("\n");

    while (true) {
        set_color(DARKGREEN);
        printf("MiniOS");
        set_color(BLUE);
        printf(" $ ");
        set_color(WHITE);
        if (gets(buf) && strlen(buf) != 0) {
            if (strcmp(buf, "help") == 0) {
                set_color(CYAN);
                printf("HELP MENU\n");
                set_color(WHITE);
                printf("use ");
                set_color(YELLOW);
                printf("orange/xxx.bin ");
                set_color(WHITE);
                printf("to execute a user file\nuse ");
                set_color(YELLOW);
                printf("net ");
                set_color(WHITE);
                printf("to try the net\n\n");
            } else if (strcmp(buf, "net") == 0) {
                printf("trying net now...\n");
                net_test_shell(6, (u8 *)"hello\0");
                printf("\n");
            } else if (strcmp(buf, "exit") == 0) {
                printf("exiting now.\n");
                return 0;
            } else if (exec(buf) != 0) {
                set_color(RED);
                printf("exec failed: file not found!\n");
                printf("you can try ");
                set_color(YELLOW);
                printf("help ");
                set_color(RED);
                printf("for help.\n\n");
                set_color(WHITE);
                continue;
            }
        }
    }
}
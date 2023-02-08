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

            char* head = strtok(buf, " ");

            // help
            if (strcmp(head, "help") == 0) {
                set_color(CYAN);
                printf("HELP MENU\n");
                set_color(WHITE);
                printf("use ");
                set_color(YELLOW);
                printf("orange/xxx.bin ");
                set_color(WHITE);
                printf("to execute a user file\nuse ");
                set_color(YELLOW);
                printf("net / arp ");
                set_color(WHITE);
                printf("to try the net / arp\n\n");
            
            // net test
            } else if (strcmp(head, "net") == 0) {
                printf("trying net now...\n");
                char* res = strtok(NULL, " ");
                net_test_shell(strlen(res), (u8 *)res);
                printf("\n");
            
            // arp
            } else if (strcmp(buf, "arp") == 0) {
                // printf("trying arp now...\n");
                net_arp();
                printf("\n");
            
            // exit
            } else if (strcmp(head, "exit") == 0) {
                printf("exiting now.\n");
                return 0;
            
            // error
            } else if (exec(head) != 0) {
                set_color(RED);
                printf("file or order name not found\n");
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
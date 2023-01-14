#include <kernlib/stdio.h>
#include <kernlib/string.h>

int disp_pos;
int saved_color = WHITE;  // 初始化为黑底白字

void kern_display_integer(int input)
{
    char output[16];
    itoa(output, input);
    kern_display_string(output);
}
#include <userlib/stdlib.h>

void delay(int time)
{
    int i, j, k;
    for (k = 0; k < time; k++) {
        // for (i = 0; i < 10000; i++) {  // for Virtual PC
        for (i = 0; i < 10; i++) {  // for Bochs
            for (j = 0; j < 10000; j++) {}
        }
    }
}
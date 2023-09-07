#include <ti/screen.h>
#include <ti/getcsc.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main(void) {
    clock_t prev = clock();
    while (os_GetCSC() != sk_Clear) {
        if (clock() - prev > CLOCKS_PER_SEC / 2) {
            char str[20];
            os_ClrHome();
            prev = clock();
            sprintf(str, "%u", clock() * 1000 / CLOCKS_PER_SEC);
            os_PutStrFull(str);
        }
    }

    return 0;
}
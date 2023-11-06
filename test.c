#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main()
{
    uint32_t a=0x1010;
    for (int i=0; i<32; i++){
        printf("%d", (a&(1<<i))>>i);
    }
    uint32_t b=(a&(1<<5))>>5;
    //return (x & (1 << n)) >> n;
    printf("%d\n", b);
}
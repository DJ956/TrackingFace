#include <stdio.h>

int main(void)
{
    printf("Hello\r\n");

    int data = 0b01011010;
    int mask = 0x80;

    if (mask & data)
    {
        printf("A\r\n");
    }
    else
    {
        printf("B\r\n");
    }

    data = data & ~mask;
    printf("DATA:%d\r\n", data);

    return 0;
}
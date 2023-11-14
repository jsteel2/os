#include "uart.h"

void uart_write(uint8_t c)
{
    UART[0] = c;
}

uint8_t uart_read(void)
{
    if ((UART[5] & 1) == 0) return 0;
    return UART[0];
}

void uart_init(void)
{
    UART[0] = 0;
    UART[1] = 1;
}


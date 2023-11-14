#ifndef _UART_H
#define _UART_H

#include <stdint.h>

#define UART ((volatile uint8_t *)0x10000000)

void uart_write(uint8_t c);
uint8_t uart_read(void);
void uart_init(void);


#endif

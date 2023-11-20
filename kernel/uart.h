#ifndef _UART_H
#define _UART_H

#include "stdint.h"

extern u8 *uart;
extern i32 uart_interrupt;

void uart_init(void *fdt);
void uart_write(u8 c);

#endif

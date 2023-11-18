#ifndef _UART_H
#define _UART_H

#include "stdint.h"

extern u8 *uart;

void uart_init(void *fdt);
void uart_write(u8 c);

#endif

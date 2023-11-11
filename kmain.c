#include <stdint.h>
#include "page.h"

void uart_write(uint8_t c)
{
    *((volatile uint8_t *)0x10000000) = c;
}

uint8_t uart_read()
{
    if ((*((volatile uint8_t *)0x10000005) & 1) == 0) return 0;
    return *((volatile uint8_t *)0x10000000);
}

void kmain(void)
{
    page_init();
    virt_enable(&kernel_table);

    char *s = "Hello World!\r\n";
    uint8_t *n = virt_page_alloc(1);
    for (int i = 0; s[i]; i++) n[i] = s[i];
    do uart_write(*n); while (*++n);
    virt_page_free(n);

    for (;;) uart_write(uart_read());
}

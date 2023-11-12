#include <stdint.h>
#include "page.h"
#include "virt.h"

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
    virt_enable();

    char *s = "Hello World!\r\n";
    uint8_t *n = virt_pages_alloc(&kernel_table, 1, ENTRY_R | ENTRY_W);
    for (int i = 0; s[i]; i++) n[i] = s[i];
    do uart_write(*n); while (*++n);

    for (;;) uart_write(uart_read());
}

#include <stdint.h>
#include "page.h"
#include "virt.h"
#include "kprint.h"

void kmain(void)
{
    page_init();
    virt_enable();

    char *s = "Hello World!";
    uint8_t *n = virt_pages_alloc(&kernel_table, 1, ENTRY_R | ENTRY_W);
    for (int i = 0; s[i]; i++) n[i] = s[i];
    kprintf("VMM Page: %s, Address: 0x%x\r\n", n, n);
    virt_pages_free(&kernel_table, n);
    kprintf("Now, Page Fault: %s", n);

    for (;;);
}

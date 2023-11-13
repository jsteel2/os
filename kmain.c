#include <stdint.h>
#include "page.h"
#include "virt.h"
#include "kprint.h"

extern size_t MEMORY_START;
extern size_t MEMORY_END;

void kmain(void)
{
    page_init();
    virt_enable();

    char *s = "Hello World!";
    uint8_t *n = virt_pages_alloc(&kernel_table, 1, ENTRY_R | ENTRY_W);
    for (int i = 0; s[i]; i++) n[i] = s[i];
    kprintf("VMM Page: %s, Virtual Address: 0x%x, Physical Address: 0x%x\r\n", n, n, *virt_page_get(&kernel_table, (size_t)n, 2, PAGE_GET) << 2);
    kprintf("Physical allocations start at: 0x%x\r\n", alloc_start);
    size_t mem_used = (size_t)alloc_start - MEMORY_START;
    for (uint8_t *p = page_start; p < alloc_start; p++)
    {
        if (*p != 0xff)
        {
            uint8_t bit_mask = 0x80;
            while (*p & bit_mask)
            {
                mem_used += PAGE_SIZE;
                bit_mask >>= 1;
            }
        }
    }
    kprintf("And end at 0x%x\r\n", alloc_end);
    kprintf("Total of %dKiB physical memory used.\r\n", mem_used / 1024);
    kprintf("Total of %dMiB physical memory free.\r\n", (HEAP_SIZE - mem_used) / 1024 / 1024);
    kprintf("Total of %dMiB physical memory in total.\r\n", (MEMORY_END - MEMORY_START) / 1024 / 1024);
    virt_pages_free(&kernel_table, n);
    kprintf("Now, Page Fault: %s", n);

    for (;;);
}

#include <stdint.h>
#include "page.h"
#include "virt.h"
#include "kprint.h"

extern size_t _memory_start;
extern size_t _memory_end;

void kmain(void)
{
    page_init();
    virt_enable();

    char *s = "Hello World!";
    uint8_t *n = virt_pages_alloc(&kernel_table, 1, ENTRY_R | ENTRY_W);
    for (int i = 0; s[i]; i++) n[i] = s[i];
    kprintf("VMM Page: %s, Virtual Address: 0x%x, Physical Address: 0x%x\r\n", n, n, *virt_page_get(&kernel_table, (size_t)n, 2) << 2);
    kprintf("Physical allocations start at: 0x%x\r\n", alloc_start);
    size_t mem_used = 0;
    for (uint8_t *p = page_start; p < alloc_start; p++)
    {
        if (*p == PAGE_TAKEN) mem_used += PAGE_SIZE;
    }
    kprintf("And end at 0x%x\r\n", alloc_end);
    kprintf("Total of %dKiB physical memory used.\r\n", mem_used / 1024);
    kprintf("Total of %dMiB physical memory free.\r\n", ((size_t)&_heap_size - mem_used) / 1024 / 1024);
    kprintf("Total of %dMiB physical memory in total.\r\n", ((size_t)&_memory_end - (size_t)&_memory_start) / 1024 / 1024);
    virt_pages_free(&kernel_table, n);
    kprintf("Now, Page Fault: %s", n);

    for (;;);
}

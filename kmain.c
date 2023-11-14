#include <stdint.h>
#include "page.h"
#include "virt.h"
#include "kprint.h"
#include "trap.h"
#include "plic.h"
#include "uart.h"
#include "kmem.h"

extern size_t MEMORY_START;
extern size_t MEMORY_END;

void kmain(void)
{
    page_init();
    clint_set();
    plic_init();
    uart_init();
    virt_enable();

    char *s = "Hello World!";
    uint8_t *n = malloc(sizeof(s));
    for (int i = 0; s[i]; i++) n[i] = s[i];
    kprintf("VMM Page: %s, Virtual Address: 0x%x, Physical Address: 0x%x\r\n", n, n, (*virt_page_get(&kernel_table, (size_t)n, 2, PAGE_GET) << 2 & ~0x3ff) + ((size_t)n & 0xfff));
    kprintf("Physical allocations start at: 0x%x\r\n", alloc_start);
    size_t mem_used = (size_t)alloc_start - MEMORY_START;
    for (uint8_t *p = page_start; p < alloc_start; p++)
    {
        if (*p)
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
    free(n);

    // steps:
    // also we need locking man
    // make a VFS and implement devfs (/dev/uart, no /dev/stdin /dev/stdout yet)
    // make tmpfs
    // mount tmpfs at / and devfs at /dev
    // write our /init executable to tmpfs /init
    // make a process, switch to user mode and execute that shit
    // make the /init process open /dev/uart, write hello to it, sleep 2 seconds then exit which should
    // make the kernel panic

    for (;;) asm volatile("wfi");
}

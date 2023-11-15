#include <stdint.h>
#include "page.h"
#include "virt.h"
#include "kprint.h"
#include "trap.h"
#include "plic.h"
#include "uart.h"
#include "kmem.h"
#include "process.h"

extern size_t MEMORY_START;
extern size_t MEMORY_END;
extern size_t STACK_START;
extern size_t STACK_END;

static inline void userspace_syscall(uint64_t a, uint64_t b, uint64_t c, uint64_t d)
{
    asm volatile("ecall");
}

void  __attribute__((aligned(4096))) userspace_init(void)
{
    userspace_syscall(64, 1, (uint64_t)"hello\r\n", 7);
    userspace_syscall(69, 3, 0, 0);
    userspace_syscall(64, 1, (uint64_t)"cool!\r\n", 7);
    userspace_syscall(93, 0, 0, 0);
}

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
        if (*p) mem_used += PAGE_SIZE;
    }
    kprintf("And end at 0x%x\r\n", alloc_end);
    kprintf("Total of %dKiB physical memory used.\r\n", mem_used / 1024);
    kprintf("Total of %dMiB physical memory free.\r\n", (HEAP_SIZE - mem_used) / 1024 / 1024);
    kprintf("Total of %dMiB physical memory in total.\r\n", (MEMORY_END - MEMORY_START) / 1024 / 1024);
    free(n);

    static PageTable table = {0};
    static Process proc;
    static uint8_t stack[4096] __attribute__((aligned(4096)));
    proc.frame = (Frame){0};
    proc.stack = (uint8_t *)STACK_END;
    proc.frame.regs[2] = STACK_ADDR + STACK_END - STACK_START;
    proc.frame.pc = 0x1000;
    proc.pid = 1;
    proc.page_table = &table;
    proc.state = PROCESS_RUNNING;
    proc.next = &proc;
    virt_range_map(&table, STACK_ADDR, (size_t)stack, sizeof(stack), ENTRY_U | ENTRY_R | ENTRY_W);
    virt_range_map(&table, proc.frame.pc, (size_t)userspace_init, 8192, ENTRY_U | ENTRY_R | ENTRY_X);
    add_process(&proc);

    asm volatile("ecall"); // start scheduling user processes

    // steps:
    // also we need locking man
    // make a VFS and implement devfs (/dev/uart, no /dev/stdin /dev/stdout yet)
    // make tmpfs
    // mount tmpfs at / and devfs at /dev
    // write our /init executable to tmpfs /init
    // make a process, switch to user mode and execute that shit
    // make the /init process open /dev/uart, write hello to it, sleep 2 seconds then exit which should
    // make the kernel panic
}

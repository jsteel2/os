#include <stdint.h>
#include "page.h"
#include "virt.h"
#include "kprint.h"
#include "trap.h"
#include "plic.h"
#include "uart.h"
#include "kmem.h"
#include "process.h"
#include "syscall.h"

extern size_t MEMORY_START;
extern size_t MEMORY_END;
extern size_t STACK_START;
extern size_t STACK_END;

static inline void userspace_syscall(uint64_t a, uint64_t b)
{
    asm volatile("mv a0, %0" :: "r"(a));
    asm volatile("mv a1, %0" :: "r"(b));
    asm volatile("ecall");
}

// TODO: handle userspace faults by killing the process instead of halting everything
void  __attribute__((aligned(4096))) userspace_init(void)
{
    char x[8];
    x[0] = 'h';
    x[1] = 'e';
    x[2] = 'l';
    x[3] = 'l';
    x[4] = '\r';
    x[5] = '\n';
    x[6] = 0;
    userspace_syscall(SYS_WRITE, (uint64_t)x);
    userspace_syscall(SYS_SLEEP, 3);
    for (int i = 5; i >= 0; i--) x[i + 1] = x[i];
    x[1] = 'H';
    x[2] = 'E';
    x[3] = 'L';
    x[4] = 'L';
    userspace_syscall(SYS_WRITE, (uint64_t)&x[1]);
    userspace_syscall(SYS_EXIT, 0);
}

void kmain(void)
{
    page_init();
    plic_init();
    virt_enable();

    char *s = "Hello World!";
    uint8_t *n = malloc(sizeof(s));
    for (int i = 0; s[i]; i++) n[i] = s[i];
    kprintf("VMM Page: %s, Virtual Address: 0x%x, Physical Address: 0x%x\r\n", n, n, virt_to_phys(&kernel_table, (size_t)n));
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
    proc.sys_frame = (Frame){0};
    proc.stack = (uint8_t *)STACK_END;
    proc.sys_stack = malloc(612); // something is fucked with kmem, it page faults on high allocations
    proc.frame.regs[2] = STACK_ADDR + STACK_END - STACK_START;
    proc.frame.pc = 0x1000;
    proc.pid = 1;
    proc.page_table = &table;
    proc.state = PROCESS_RUNNING;
    proc.next = &proc;
    proc.prev = &proc;
    proc.mode = PROCESS_USER;
    virt_range_map(&table, STACK_ADDR, (size_t)stack, sizeof(stack), ENTRY_U | ENTRY_R | ENTRY_W);
    virt_range_map(&table, proc.frame.pc, (size_t)userspace_init, 8192, ENTRY_U | ENTRY_R | ENTRY_X);
    add_process(&proc);

    clint_set();
    uart_init();

    asm volatile("li a0, 1\n\tecall"); // start scheduling user processes

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

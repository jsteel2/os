#include "trap.h"
#include "sbi.h"
#include "uart.h"
#include "kprint.h"
#include <libfdt.h>

void kmain(void *fdt)
{
    uart_init(fdt);

    kprintf("good morning sirs\r\n");

    sbi_call(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, 0);
    asm volatile("li t1, 0xaaa\n\tcsrw sie, t1");
    asm volatile("mv t0, %0\n\tcsrw stvec, t0" :: "r"(trap_vector));
    asm volatile("csrrsi zero, sstatus, 1 << 1");
    for (;;);
}

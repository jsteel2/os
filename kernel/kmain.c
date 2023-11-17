#include "trap.h"
#include "sbi.h"

void kmain(void *fdt)
{
    sbi_call(0x1, 0, 'H');
    sbi_call(0x54494D45, 0, 0);
    asm volatile("li t1, 0xaaa\n\tcsrw sie, t1");
    asm volatile("mv t0, %0\n\tcsrw stvec, t0" :: "r"(trap_vector));
    asm volatile("csrrsi zero, sstatus, 1 << 1");
    for (;;);
}

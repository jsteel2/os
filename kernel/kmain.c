#include "trap.h"
#include "sbi.h"
#include "uart.h"
#include "kprint.h"
#include "pmm.h"
#include "entry.h"
#include "time.h"
#include <libfdt.h>

void kmain_hart(usize hart)
{
    kprintf("good morning from hartid %d", hart);
    for (;;);
}

void kmain(usize hart, void *fdt)
{
    uart_init(fdt);
    time_init(fdt);
    pmm_init(fdt);

    int node_offset = -1;
    while ((node_offset = fdt_node_offset_by_compatible(fdt, node_offset, "riscv")) >= 0)
    {
        const u32 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
        if (!prop) continue;
        u32 cpu = fdt32_to_cpu(prop[0]);
        if (cpu != hart)
        {
            usize x = 1;
            sbi_call(SBI_EXT_HSM, SBI_EXT_HSM_HART_START, cpu, (u64)hart_entry, (u64)pmm_alloc(&x));
        }
    }

    kprintf("good morning from hartid %d", hart);

    sbi_call(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, 0, 0, 0);
    asm volatile("li t1, 0xaaa\n\tcsrw sie, t1");
    asm volatile("mv t0, %0\n\tcsrw stvec, t0" :: "r"(trap_vector));
    asm volatile("csrrsi zero, sstatus, 1 << 1");
    for (;;);
}

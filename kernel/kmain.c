#include "trap.h"
#include "sbi.h"
#include "uart.h"
#include "kprint.h"
#include "pmm.h"
#include "entry.h"
#include "time.h"
#include "plic.h"
#include "vmm.h"
#include "virtio.h"
#include <libfdt.h>

void kmain_hart(usize hart)
{
    kprintf("good morning from hartid %d", hart);
    plic_start_hart(hart);
    vmm_enable(&kernel_vmm_table);
    sbi_call(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, 0, 0, 0);
    enable_interrupts();
    for (;;) sbi_call(SBI_EXT_HSM, SBI_EXT_HSM_HART_SUSPEND, 0, 0, 0);
}

void kmain(usize hart, void *fdt)
{
    uart_init(fdt);
    time_init(fdt);
    plic_init(fdt);
    plic_start_hart(hart);
    pmm_init(fdt);
    virtio_init(fdt);
    vmm_init();
    vmm_enable(&kernel_vmm_table);

    int node_offset = -1;
    while ((node_offset = fdt_node_offset_by_compatible(fdt, node_offset, "riscv")) >= 0)
    {
        const u32 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
        if (!prop) continue;
        u32 cpu = fdt32_to_cpu(prop[0]);
        if (cpu != hart)
        {
            sbi_call(SBI_EXT_HSM, SBI_EXT_HSM_HART_START, cpu, (u64)hart_entry, (u64)pmm_alloc_continuous(1));
        }
    }

    kprintf("good morning from boot hartid %d", hart);

    sbi_call(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, 0, 0, 0);
    enable_interrupts();
    for (;;) sbi_call(SBI_EXT_HSM, SBI_EXT_HSM_HART_SUSPEND, 0, 0, 0);
}

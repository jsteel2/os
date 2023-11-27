#include "plic.h"
#include "stdint.h"
#include "kprint.h"
#include "uart.h"
#include <libfdt.h>

u64 plic;
usize plic_size;

void plic_init(void *fdt)
{
    int node_offset = fdt_node_offset_by_compatible(fdt, -1, "riscv,plic0");
    if (node_offset < 0) goto err;

    const u32 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
    if (!prop) goto err;

    plic = ((u64)fdt32_to_cpu(prop[0]) << 32) | fdt32_to_cpu(prop[1]);
    plic_size = ((u64)fdt32_to_cpu(prop[2]) << 32) | fdt32_to_cpu(prop[3]);

    if (uart_interrupt >= 0) plic_set_priority(uart_interrupt, 1);
    // dont like that we're hardcoding the virtio mmio interrupt numbers
    // we can get them from the fdt instead
    for (usize i = 1; i <= 8; i++) plic_set_priority(i, 1);
    return;

err:
    kprintf("Could not initialize PLIC.");
    for (;;);
}

void plic_enable(u32 id, usize hart)
{
    u32 *p = (u32 *)(plic + 0x2080 + hart * 0x100);
    *p |= 1 << id;
}

void plic_set_priority(u32 id, u8 priority)
{
    u32 *p = (u32 *)plic;
    p[id] = priority & 7;
}

void plic_set_threshold(u8 threshold, usize hart)
{
    u32 *p = (u32 *)(plic + 0x200000 + 0x1000 + hart * 0x2000);
    *p = threshold & 7;
}

u32 plic_next(usize hart)
{
    u32 *p = (u32 *)(plic + 0x200004 + 0x1000 + hart * 0x2000);
    return *p;
}

void plic_claim(usize hart, u32 id)
{
    u32 *p = (u32 *)(plic + 0x200004 + 0x1000 + hart * 0x2000);
    *p = id;
}

void plic_start_hart(usize hart)
{
    plic_set_threshold(0, hart);
    for (usize i = 1; i <= 8; i++) plic_enable(i, hart);
    if (uart_interrupt >= 0)
    {
        plic_enable(uart_interrupt, hart);
    }
}

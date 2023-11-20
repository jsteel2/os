#include "uart.h"
#include <libfdt.h>

u8 *uart = NULL;
i32 uart_interrupt = -1;

void uart_init(void *fdt)
{
    int node_offset = fdt_node_offset_by_compatible(fdt, -1, "ns16550a");
    if (node_offset < 0) return;

    const u32 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
    if (!prop) return;

    uart = (u8 *)(((u64)fdt32_to_cpu(prop[0]) << 32) | fdt32_to_cpu(prop[1]));

    // enable FIFO, interrupts and divisor rate stuff
    uart[3] = 3;
    uart[2] = 1;
    uart[1] = 1;

    u16 divisor = 592;
    uart[3] = 3 | (1 << 7);
    uart[0] = divisor & 0xff;
    uart[1] = divisor >> 8;
    uart[3] = 3;

    prop = fdt_getprop(fdt, node_offset, "interrupts", NULL);
    if (!prop) return;

    uart_interrupt = fdt32_to_cpu(prop[0]);

    uart[0] = 0;
    uart[1] = 1;
}

void uart_write(u8 c)
{
    if (uart) *uart = c;
}

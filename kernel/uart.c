#include "uart.h"
#include <libfdt.h>

u8 *uart = NULL;

void uart_init(void *fdt)
{
    int node_offset = fdt_node_offset_by_compatible(fdt, -1, "ns16550a");
    if (node_offset < 0) return;

    const u64 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
    if (!prop) return;

    uart = (u8 *)fdt64_to_cpu(prop[0]);
}

void uart_write(u8 c)
{
    if (uart) *uart = c;
}

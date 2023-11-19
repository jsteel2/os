#include "time.h"
#include "kprint.h"
#include <libfdt.h>

u32 timebase_frequency;

void time_init(void *fdt)
{
    int node_offset = fdt_path_offset(fdt, "/cpus");
    if (node_offset < 0) goto err;

    const u32 *prop = fdt_getprop(fdt, node_offset, "timebase-frequency", NULL);
    if (!prop) goto err;

    timebase_frequency = fdt32_to_cpu(prop[0]);
    return;

err:
    kprintf("Could not get timebase-frequency");
    for (;;);
}

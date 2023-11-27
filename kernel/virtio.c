#include "virtio.h"
#include "stdint.h"
#include "vmm.h"
#include "kprint.h"
#include "virtio_disk.h"
#include <libfdt.h>

void virtio_init_device(u64 addr)
{
    if (*VR(addr, VIRTIO_MMIO_MAGIC_VALUE) != VIRTIO_MAGIC_VALUE || *VR(addr, VIRTIO_MMIO_VERSION) != 1) return;

    switch (*VR(addr, VIRTIO_MMIO_VENDOR_ID))
    {
        case 0x554d4551:
            switch (*VR(addr, VIRTIO_MMIO_DEVICE_ID))
            {
                case 2: virtio_init_disk(addr); break;
            }
            break;
    }
}

void virtio_init(void *fdt)
{
    int node_offset = -1;
    while ((node_offset = fdt_node_offset_by_compatible(fdt, node_offset, "virtio,mmio")) >= 0)
    {
        const u32 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
        if (!prop) continue;
        u64 addr = ((u64)fdt32_to_cpu(prop[0]) << 32) | fdt32_to_cpu(prop[1]);
        u64 size = ((u64)fdt32_to_cpu(prop[2]) << 32) | fdt32_to_cpu(prop[3]);
        vmm_identity_map(&kernel_vmm_table, addr, size, ENTRY_R | ENTRY_W);
        virtio_init_device(addr);
    }
}

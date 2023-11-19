#include "pmm.h"
#include "kprint.h"
#include "stdint.h"
#include "kmem.h"
#include "lock.h"
#include <libfdt.h>

usize pages;
u8 *page_map;
u64 pmem_start;
usize start = 0;

void pmm_init(void *fdt)
{
    int node_offset = fdt_node_offset_by_prop_value(fdt, -1, "device_type", "memory", sizeof("memory"));
    if (node_offset < 0) goto err;

    const u32 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
    if (!prop) goto err;

    pmem_start = ((u64)fdt32_to_cpu(prop[0]) << 32) | fdt32_to_cpu(prop[1]);
    u64 pmem_size = ((u64)fdt32_to_cpu(prop[2]) << 32) | fdt32_to_cpu(prop[3]);

    page_map = (u8 *)KERNEL_END;
    pages = pmem_size / PAGE_SIZE;
    memset(page_map, 0, pages);

    memset(page_map + (KERNEL_START - pmem_start) / PAGE_SIZE, 1, (KERNEL_END - KERNEL_START) / PAGE_SIZE);
    memset(page_map + ((u64)page_map - pmem_start) / PAGE_SIZE, 1, pages / PAGE_SIZE);

    node_offset = fdt_path_offset(fdt, "/reserved-memory");
    if (node_offset < 0) return;
    node_offset = fdt_first_subnode(fdt, node_offset);
    while (node_offset >= 0)
    {
        const u32 *prop = fdt_getprop(fdt, node_offset, "reg", NULL);
        if (!prop) continue;
        u64 reserved_addr = ((u64)fdt32_to_cpu(prop[0]) << 32) | fdt32_to_cpu(prop[1]);
        u64 reserved_size = ((u64)fdt32_to_cpu(prop[2]) << 32) | fdt32_to_cpu(prop[3]);
        memset(page_map + (reserved_addr - pmem_start) / PAGE_SIZE, 1, reserved_size / PAGE_SIZE + 1);
        // dont know why but wiithout the + 1 there we store fault when accessing the page that should be free
        node_offset = fdt_next_subnode(fdt, node_offset);
    }

    return;
err:
    kprintf("Could not initialize PMM.");
    for (;;);
}

static Lock lock = 0;

void *pmm_alloc(usize *n)
{
    // make this into a bitmap someday
    lock_acquire(&lock);
    for (; start < pages; start++)
    {
        if (!page_map[start])
        {
            void *r = (void *)pmem_start + start * PAGE_SIZE;
            do
            {
                page_map[start] = 1;
                start++;
                (*n)--;
            } while (!page_map[start] && *n);
            lock_release(&lock);
            return r;
        }
    }

    lock_release(&lock);
    return NULL;
}

void pmm_free(void *p, usize n)
{
    usize loc = ((usize)p - pmem_start) / PAGE_SIZE;
    for (usize i = 0; i < n; i++) page_map[i + loc] = 0;
    start = loc;
}

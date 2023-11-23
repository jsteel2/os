#include "vmm.h"
#include "pmm.h"
#include "kmem.h"
#include "uart.h"
#include "plic.h"

// add a per-table lock... somewhere

VMMTable kernel_vmm_table = {0};

u64 *vmm_walk(VMMTable *table, usize vaddr, PageLevel level, bool alloc)
{
    u16 vpn[] = {(vaddr >> 12) & 0x1ff, (vaddr >> 21) & 0x1ff, (vaddr >> 30) & 0x1ff};
    for (usize i = 2; i > level; i--)
    {
        if ((table->entries[vpn[i]] & 0xf) > ENTRY_V) return &table->entries[vpn[i]];

        if (!(table->entries[vpn[i]] & ENTRY_V))
        {
            if (!alloc) return NULL;
            usize x = 1;
            u8 *page = pmm_alloc(&x);
            memset(page, 0, PAGE_SIZE);
            table->entries[vpn[i]] = ((usize)page >> 2) | ENTRY_V;
        }

        table = (VMMTable *)((table->entries[vpn[i]] & ~0x3ff) << 2);
    }
    return &table->entries[vpn[level]];
}

void vmm_page_map(VMMTable *table, usize vaddr, usize paddr, u64 bits, PageLevel level)
{
    u16 ppn[] = {(paddr >> 12) & 0x1ff, (paddr >> 21) & 0x1ff, (paddr >> 30) & 0x3ffffff};
    u64 *entry = vmm_walk(table, vaddr, level, true);
    *entry = (ppn[2] << 28) | (ppn[1] << 19) | (ppn[0] << 10) | bits | ENTRY_V;
}

void vmm_range_map(VMMTable *table, usize vstart, usize pstart, ssize len, u64 bits)
{
    usize vaddr = vstart & ~(PAGE_SIZE - 1);
    usize paddr = pstart & ~(PAGE_SIZE - 1);

    while (len > 0)
    {
        u64 n = PAGE_SIZE;

        if (len >= PAGE_1G_SIZE && (vaddr & (PAGE_1G_SIZE - 1)) == 0)
        {
            vmm_page_map(table, vaddr, paddr, bits, PAGE_LEVEL_1G);
            n = PAGE_1G_SIZE;
        }
        else if (len >= PAGE_2M_SIZE && (vaddr & (PAGE_2M_SIZE - 1)) == 0)
        {
            vmm_page_map(table, vaddr, paddr, bits, PAGE_LEVEL_2M);
            n = PAGE_2M_SIZE;
        }
        else
        {
            vmm_page_map(table, vaddr, paddr, bits, PAGE_LEVEL_4K);
        }

        vaddr += n;
        paddr += n;
        len -= n;
    }
}

void vmm_identity_map(VMMTable *table, usize start, usize len, u64 bits)
{
    vmm_range_map(table, start, start, len, bits);
}

void vmm_init(void)
{
    vmm_identity_map(&kernel_vmm_table, pmem_start, pmm_pages * PAGE_SIZE, ENTRY_R | ENTRY_W | ENTRY_X);
    vmm_identity_map(&kernel_vmm_table, (usize)uart, uart_size, ENTRY_R | ENTRY_W);
    vmm_identity_map(&kernel_vmm_table, plic, plic_size, ENTRY_R | ENTRY_W);
}

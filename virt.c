#include "virt.h"
#include "page.h"

PageTable kernel_table;

uint64_t *virt_page_get(PageTable *table, size_t vaddr, int level)
{
    uint16_t vpn[] = {(vaddr >> 12) & 0x1ff, (vaddr >> 21) & 0x1ff, (vaddr >> 30) & 0x1ff};

    if (level == 0) return &table->entries[vpn[0]];

    if (!(table->entries[vpn[level]] & ENTRY_V))
    {
        size_t x = 1;
        uint8_t *y = NULL;
        uint8_t *page = page_alloc(&x, &y);
        for (int i = 0; i < PAGE_SIZE; i++) page[i] = 0;
        table->entries[vpn[level]] = ((size_t)page >> 2) | ENTRY_V;
    }
    PageTable *next = (PageTable *)((table->entries[vpn[level]] & ~0x3ff) << 2);
    return virt_page_get(next, vaddr, level - 1);
}

void virt_page_map(PageTable *table, size_t vaddr, size_t paddr, uint64_t bits)
{
    uint16_t ppn[] = {(paddr >> 12) & 0x1ff, (paddr >> 21) & 0x1ff, (paddr >> 30) & 0x3ffffff};
    uint64_t *entry = virt_page_get(table, vaddr, 2);
    *entry = (ppn[2] << 28) | (ppn[1] << 19) | (ppn[0] << 10) | bits | ENTRY_V;
}

void virt_page_unmap(PageTable *table, size_t vaddr)
{
    // leaky because we never free the page tables other than when we free the whole table
    uint64_t *entry = virt_page_get(table, vaddr, 2);
    *entry = 0;
}

void virt_table_free(PageTable *table)
{
    // TODO
}

void virt_range_map(PageTable *table, size_t vstart, size_t pstart, size_t length, uint64_t bits)
{
    size_t vaddr = vstart & ~(PAGE_SIZE - 1);
    size_t paddr = pstart & ~(PAGE_SIZE - 1);

    for (size_t i = 0; i <= length; i += PAGE_SIZE, vaddr += PAGE_SIZE, paddr += PAGE_SIZE)
    {
        virt_page_map(table, vaddr, paddr, bits);
    }
}

void virt_identity_map(PageTable *table, size_t start, size_t end, uint64_t bits)
{
    virt_range_map(table, start, start, end - start, bits);
}

void virt_enable()
{
    for (size_t i = 0; i < sizeof(PageTable) / sizeof(*kernel_table.entries); i++) kernel_table.entries[i] = 0;

    virt_identity_map(&kernel_table, (size_t)&_text_start, (size_t)&_text_end, ENTRY_R | ENTRY_X);
    virt_identity_map(&kernel_table, (size_t)&_bss_start, (size_t)&_bss_end, ENTRY_R | ENTRY_W);
    virt_identity_map(&kernel_table, (size_t)&_rodata_start, (size_t)&_rodata_end, ENTRY_R);
    virt_identity_map(&kernel_table, (size_t)&_data_start, (size_t)&_data_end, ENTRY_R | ENTRY_W);

    virt_identity_map(&kernel_table, (size_t)page_start, (size_t)alloc_end, ENTRY_R | ENTRY_W);

    virt_identity_map(&kernel_table, 0x10000000, 0x10000005, ENTRY_R | ENTRY_W);

    asm_virt_enable(((size_t)&kernel_table >> 12) | ((size_t)8 << 60));
}

size_t virt_pages_find(PageTable *table, size_t pages)
{
    // this could be optimized by doing the old function (just nicer), it should still be in git logs under virt_page_alloc
    size_t continuous = 0;
    size_t vaddr = 0;

    for (size_t i = PAGE_SIZE; continuous < pages; i += PAGE_SIZE)
    {
        if (continuous == 0) vaddr = i;
        if (*virt_page_get(table, i, 2) & ENTRY_V) continuous = 0;
        else continuous++;
    }

    return vaddr;
}

uint8_t *virt_pages_alloc(PageTable *table, size_t pages, uint64_t bits)
{
    size_t vaddr = virt_pages_find(table, pages);

    uint8_t *start = NULL;
    size_t i = vaddr;
    while (pages)
    {
        size_t b = pages;
        size_t paddr = (size_t)page_alloc(&pages, &start);
        size_t alloced = b - pages;
        if (pages == 0)
        {
            if (alloced > 1) virt_range_map(table, i, paddr, (alloced - 1) * PAGE_SIZE, bits);
            virt_page_map(table, i + (alloced - 1) * PAGE_SIZE, paddr + (alloced - 1) * PAGE_SIZE, bits | ENTRY_L);
        }
        else
        {
            virt_range_map(table, i, paddr, alloced * PAGE_SIZE, bits);
        }
        i += alloced * PAGE_SIZE;
    }

    return (uint8_t *)vaddr;
}

void virt_pages_free(PageTable *table, uint8_t *vaddr)
{
    uint64_t *entry;
    while (!(*(entry = virt_page_get(table, (size_t)vaddr, 2)) & ENTRY_L))
    {
        *entry = 0;
        vaddr += PAGE_SIZE;
    }
}

#include "page.h"

uint8_t *page_start;
uint8_t *alloc_start;
PageTable kernel_table;

static size_t align_order(size_t val, size_t order)
{
    size_t o = (1 << order) - 1;
    return (val + o) & ~o;
}

void page_init()
{
    page_start = (uint8_t *)&_heap_start;

    size_t num_pages = (size_t)&_heap_size / PAGE_SIZE;
    for (size_t i = 0; i < num_pages; i++) page_start[i] = PAGE_FREE;

    alloc_start = (uint8_t *)align_order((size_t)page_start + num_pages, PAGE_ORDER);
}

uint8_t *page_alloc(size_t *pages, uint8_t **start)
{
    // optimize this to use bits instead of bytes
    for (uint8_t *p = start ? *start : page_start; p < alloc_start; p++)
    {
        if (*p == PAGE_FREE)
        {
            uint8_t *ret = alloc_start + (p - page_start) * PAGE_SIZE;
            do
            {
                *p = PAGE_TAKEN;
                --*pages;
            } while (*pages > 0 && *++p == PAGE_FREE);
            *start = p;
            return ret;
        }
    }
    return NULL;
}

void page_free(uint8_t *p, size_t pages)
{
    for (int i = 0; i < pages; i++, p += PAGE_SIZE) page_start[(p - alloc_start) / PAGE_SIZE] = PAGE_FREE;
}

// seperate this into a new file virt.c

void virt_page_map(PageTable *table, size_t vaddr, size_t paddr, uint8_t bits, int level)
{
    uint16_t vpn[] = {(vaddr >> 12) & 0x1ff, (vaddr >> 21) & 0x1ff, (vaddr >> 30) & 0x1ff};
    uint16_t ppn[] = {(paddr >> 12) & 0x1ff, (paddr >> 21) & 0x1ff, (paddr >> 30) & 0x3ffffff};

    if (level == 0)
    {
        table->entries[vpn[0]] = (ppn[2] << 28) | (ppn[1] << 19) | (ppn[0] << 10) | bits | ENTRY_V;
    }
    else
    {
        if (!(table->entries[vpn[level]] & ENTRY_V))
        {
            size_t x = 1;
            uint8_t *page = page_alloc(&x, NULL);
            for (int i = 0; i < PAGE_SIZE; i++) page[i] = 0;
            table->entries[vpn[level]] = ((size_t)page >> 2) | ENTRY_V;
        }
        PageTable *next = (PageTable *)((table->entries[vpn[level]] & (~0x3ff)) << 2);
        virt_page_map(next, vaddr, paddr, bits, level - 1);
    }
}

void virt_page_unmap(PageTable *table, size_t vaddr)
{
    uint16_t vpn[] = {(vaddr >> 12) & 0x1ff, (vaddr >> 21) & 0x1ff, (vaddr >> 30) & 0x1ff};
    PageTable *lvl1 = (PageTable *)(((table->entries[vpn[2]] & (~0x3ff)) << 2) + vpn[2]);
    PageTable *lvl0 = (PageTable *)(((lvl1->entries[vpn[1]] & (~0x3ff)) << 2) + vpn[1]);
    page_free((uint8_t *)((lvl0->entries[vpn[0]] & (~0x3ff)) << 2), 1);
    lvl0->entries[vpn[0]] = 0;
}

void virt_table_free(PageTable *table)
{
    // IMPLEMENT
}

void virt_identity_map(PageTable *table, size_t start, size_t end, uint8_t bits)
{
    size_t addr = start & ~(PAGE_SIZE - 1);

    while (addr <= end >> 12 << 12)
    {
        virt_page_map(table, addr, addr, bits, 2);
        addr += PAGE_SIZE;
    }
}

void virt_enable(PageTable *table)
{
    for (int i = 0; i < sizeof(PageTable) / sizeof(*table->entries); i++) table->entries[i] = 0;

    virt_identity_map(table, (size_t)&_text_start, (size_t)&_text_end, ENTRY_R | ENTRY_X);
    virt_identity_map(table, (size_t)&_bss_start, (size_t)&_bss_end, ENTRY_R | ENTRY_W);
    virt_identity_map(table, (size_t)&_rodata_start, (size_t)&_rodata_end, ENTRY_R);
    virt_identity_map(table, (size_t)&_data_start, (size_t)&_data_end, ENTRY_R | ENTRY_W);

    virt_identity_map(table, (size_t)page_start, (size_t)alloc_start, ENTRY_R | ENTRY_W);

    virt_identity_map(table, 0x10000000, 0x10000005, ENTRY_R | ENTRY_W);

    uint64_t root_ppn = (size_t)table >> 12;
    uint64_t satp_val = ((size_t)8 << 60) | root_ppn;

    asm_virt_enable(satp_val);
}

uint8_t *virt_page_alloc(PageTable *table, size_t pages)
{
    // allocate [pages] physical pages and map them into continuous virtual memory pages
    // STEPS:
    // find continuous virtual memory space
    // set it all as taken
    // for each one, map a physical page to the virtual address

    // instead of dooing all tis shit generale virt_page_map into virt_page_get
    // and also generalize virt_identity_map into virt_range_map and use that here
    // i think this function should behave more like mmap, and should also be a higher level wrapper
    // to a function virt_page_find that gets the  continuous space
    // then here we can just virt_page_find, loop page_alloc and virt_range_map
    // and dont forget to set ENTRY_L for the last page

    size_t continuous = 0;
    size_t vaddr = 0;

    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++)
    {
        if (!(table->entries[i] & ENTRY_V))
        {
            size_t x = 1;
            uint8_t *page = page_alloc(&x, NULL);
            for (int n = 0; n < PAGE_SIZE; n++) page[n] = 0;
            table->entries[i] = ((size_t)page >> 2) | ENTRY_V;
        }
        PageTable *lvl1 = (PageTable *)((table->entries[i] & (~0x3ff)) << 2);

        for (int j = 0; j < PAGE_TABLE_ENTRIES; j++)
        {
            if (!(lvl1->entries[j] & ENTRY_V))
            {
                size_t x = 1;
                uint8_t *page = page_alloc(&x, NULL);
                for (int n = 0; n < PAGE_SIZE; n++) page[n] = 0;
                lvl1->entries[j] = ((size_t)page >> 2) | ENTRY_V;
            }
            PageTable *lvl0 = (PageTable *)((table->entries[j] & (~0x3ff)) << 2);

            for (int k = 0; k < PAGE_TABLE_ENTRIES; k++)
            {
                if (continuous == 0) vaddr = 123+456+768;
                if (!(lvl0->entries[k] & ENTRY_V)) continuous = 0;
                else continuous++;

                if (continuous == pages) goto found;
            }
        }
    }

    return NULL;
found:
    // actually allocate it
    return (uint8_t *)vaddr;
}

void virt_page_free(PageTable *table, uint8_t *p)
{
    // unmap page table entry, and free physical page
}

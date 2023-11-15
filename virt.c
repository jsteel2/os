#include "virt.h"
#include "page.h"
#include "plic.h"
#include "trap.h"
#include "uart.h"

// if we catch any paging bugs, figure out https://blog.stephenmarz.com/2021/02/01/wrong-about-sfence/
// because i dont think we're using sfence.vma correctly

PageTable kernel_table = {0};
Frame trap_frame;

uint64_t *virt_page_get(PageTable *table, size_t vaddr, unsigned level, PageSize size)
{
    uint16_t vpn[] = {(vaddr >> 12) & 0x1ff, (vaddr >> 21) & 0x1ff, (vaddr >> 30) & 0x1ff};

    if (level == size || (table->entries[vpn[level]] & 0xf) > ENTRY_V) return &table->entries[vpn[level]];

    if (!(table->entries[vpn[level]] & ENTRY_V))
    {
        size_t x = 1;
        uint8_t *y = NULL;
        uint8_t *page = page_alloc(&x, &y);
        for (int i = 0; i < PAGE_SIZE; i++) page[i] = 0;
        table->entries[vpn[level]] = ((size_t)page >> 2) | ENTRY_V;
    }
    PageTable *next = (PageTable *)((table->entries[vpn[level]] & ~0x3ff) << 2);
    return virt_page_get(next, vaddr, level - 1, size);
}

void virt_page_map(PageTable *table, size_t vaddr, size_t paddr, uint64_t bits, PageSize size)
{
    uint16_t ppn[] = {(paddr >> 12) & 0x1ff, (paddr >> 21) & 0x1ff, (paddr >> 30) & 0x3ffffff};
    uint64_t *entry = virt_page_get(table, vaddr, 2, size);
    *entry = (ppn[2] << 28) | (ppn[1] << 19) | (ppn[0] << 10) | bits | ENTRY_V;
}

void virt_page_unmap(PageTable *table, size_t vaddr)
{
    // leaky because we never free the page tables other than when we free the whole table
    uint64_t *entry = virt_page_get(table, vaddr, 2, PAGE_GET);
    *entry = 0;
    asm volatile("sfence.vma %0, zero" :: "r"(vaddr));
}

void virt_table_free(PageTable *table)
{
    // TODO
}

void virt_range_map(PageTable *table, size_t vstart, size_t pstart, int64_t length, uint64_t bits)
{
    size_t vaddr = vstart & ~(PAGE_SIZE - 1);
    size_t paddr = pstart & ~(PAGE_SIZE - 1);

    while (length >= 0)
    {
        uint64_t n = PAGE_SIZE;

        if (length >= PAGE_SIZE << 9 << 9 && (vaddr & ((PAGE_SIZE << 9 << 9) - 1)) == 0)
        {
            virt_page_map(table, vaddr, paddr, bits, PAGE_1G);
            n = PAGE_SIZE << 9 << 9;
        }
        else if (length >= PAGE_SIZE << 9 && (vaddr & ((PAGE_SIZE << 9) - 1)) == 0)
        {
            virt_page_map(table, vaddr, paddr, bits, PAGE_2M);
            n = PAGE_SIZE << 9;
        }
        else
        {
            virt_page_map(table, vaddr, paddr, bits, PAGE_4K);
        }

        vaddr += n;
        paddr += n;
        length -= n;
    }
}

void virt_identity_map(PageTable *table, size_t start, size_t end, uint64_t bits)
{
    virt_range_map(table, start, start, end - start, bits);
}

void virt_enable()
{
    virt_identity_map(&kernel_table, TEXT_START, TEXT_END, ENTRY_R | ENTRY_X);
    virt_identity_map(&kernel_table, BSS_START, BSS_END, ENTRY_R | ENTRY_W);
    virt_identity_map(&kernel_table, RODATA_START, RODATA_END, ENTRY_R);
    virt_identity_map(&kernel_table, DATA_START, DATA_END, ENTRY_R | ENTRY_W);

    virt_identity_map(&kernel_table, (size_t)page_start, (size_t)alloc_end, ENTRY_R | ENTRY_W);

    virt_identity_map(&kernel_table, (size_t)UART, (size_t)UART, ENTRY_R | ENTRY_W);

    uint64_t satp = ((size_t)&kernel_table >> 12) | ((size_t)8 << 60);

    trap_frame.satp = satp;
    
    asm_virt_enable(satp, (uint64_t)&trap_frame);
}

size_t virt_pages_find(PageTable *table, size_t pages, size_t start)
{
    // this could be optimized by doing the old function (just nicer), it should still be in git logs under virt_page_alloc
    size_t continuous = 0;
    size_t vaddr = 0;

    for (size_t i = start; continuous < pages; i += PAGE_SIZE)
    {
        if (continuous == 0) vaddr = i;
        if (*virt_page_get(table, i, 2, PAGE_GET) & ENTRY_V) continuous = 0;
        else continuous++;
    }

    return vaddr;
}

// make these compatible with huge pages, and virt_page_unmap too

uint8_t *virt_pages_alloc(PageTable *table, size_t pages, uint64_t bits)
{
    size_t vaddr = virt_pages_find(table, pages, PAGE_SIZE);

    uint8_t *start = NULL;
    size_t i = vaddr;
    while (pages)
    {
        size_t b = pages;
        size_t paddr = (size_t)page_alloc(&pages, &start);
        size_t alloced = b - pages;
        virt_range_map(table, i, paddr, alloced * PAGE_SIZE, bits);
        i += alloced * PAGE_SIZE;
    }

    return (uint8_t *)vaddr;
}

void virt_pages_free(PageTable *table, uint8_t *vaddr, size_t pages)
{
    uint64_t *entry;
    for (size_t i = 0; i < pages; i++)
    {
        entry = virt_page_get(table, (size_t)vaddr, 2, PAGE_GET);
        page_free((uint8_t *)(*entry << 2), 1);
        *entry = 0;
        asm volatile("sfence.vma %0, zero" :: "r"(vaddr));
        vaddr += PAGE_SIZE;
    }
}

#include "page.h"

uint8_t *page_start;
uint8_t *alloc_start;
uint8_t *alloc_end;

static size_t align_order(size_t val, size_t order)
{
    size_t o = (1 << order) - 1;
    return (val + o) & ~o;
}

void page_init()
{
    page_start = (uint8_t *)&_heap_start;

    size_t num_pages = (size_t)&_heap_size / PAGE_SIZE;
    for (size_t i = 0; i < num_pages / 8; i++) page_start[i] = 0;

    alloc_start = (uint8_t *)align_order((size_t)page_start + num_pages / 8, PAGE_ORDER);
    alloc_end = alloc_start + num_pages * PAGE_SIZE;
}

uint8_t *page_alloc(size_t *pages, uint8_t **start)
{
    // could be faster with more bitwise hacks or uint64_t
    for (uint8_t *p = *start ? *start : page_start; p < alloc_start; p++)
    {
        if (*p != 0xff)
        {
            uint8_t bit_mask = 0x80;
            uint8_t bit_pos = 0;
            while (*p & bit_mask)
            {
                bit_mask >>= 1;
                bit_pos++;
            }
            do
            {
                *p ^= bit_mask;
                --*pages;
                bit_mask >>= 1;
                bit_pos++;
            } while (*pages > 0 && *p ^ bit_mask && bit_mask);
            *start = p;
            return alloc_start + (p + bit_pos - page_start) * PAGE_SIZE;
        }
    }
    return NULL;
}

void page_free(uint8_t *p, size_t pages)
{
    for (size_t i = 0; i < pages; i++, p += PAGE_SIZE) page_start[(p - alloc_start) / PAGE_SIZE / 8] &= ~(0x80 >> (p - alloc_start) / PAGE_SIZE % 8);
}

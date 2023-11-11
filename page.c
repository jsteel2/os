#include "page.h"

uint8_t *page_start;
uint8_t *alloc_start;

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

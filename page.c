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
    page_start = (uint8_t *)HEAP_START;

    size_t num_pages = HEAP_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_pages ; i++) page_start[i] = 0;

    alloc_start = (uint8_t *)align_order((size_t)page_start + num_pages, PAGE_ORDER);
    alloc_end = alloc_start + num_pages * PAGE_SIZE;
}

uint8_t *page_alloc(size_t *pages, uint8_t **start)
{
    // reverted to bytes because the bits shit was buggy and i cant be fucked rn!!
    for (uint8_t *p = *start ? *start : page_start; p < alloc_start; p++)
    {
        if (*p == 0)
        {
            uint8_t *ret = alloc_start + (p - page_start) * PAGE_SIZE;
            do
            {
                *p = 1;
                --*pages;
            } while (*pages > 0 && *++p == 0);
            *start = p;
            return ret;
        }
    }
    return NULL;
}

void page_free(uint8_t *p, size_t pages)
{
    for (size_t i = 0; i < pages; i++, p += PAGE_SIZE) page_start[(p - alloc_start) / PAGE_SIZE] = 0;
}

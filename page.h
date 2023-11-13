#ifndef _PAGE_H
#define _PAGE_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_ORDER 12
#define PAGE_SIZE (1 << PAGE_ORDER)

extern size_t _heap_start;
extern size_t _heap_size;

extern uint8_t *page_start;
extern uint8_t *alloc_start;
extern uint8_t *alloc_end;

void page_init();
uint8_t *page_alloc(size_t *pages, uint8_t **start);
void page_free(uint8_t *p, size_t pages);

#endif

#ifndef _PMM_H
#define _PMM_H

#include "stdint.h"
#include "symbols.h"

#define PAGE_SIZE 4096

void pmm_init(void *fdt);
void *pmm_alloc(usize *n);
void *pmm_alloc_continuous(usize n);
void pmm_free(void *p, usize n);

extern u64 pmem_start;
extern usize pmm_pages;

#endif

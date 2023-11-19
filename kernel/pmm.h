#ifndef _PMM_H
#define _PMM_H

#include "stdint.h"

#define PAGE_SIZE 4096

extern usize KERNEL_START;
extern usize KERNEL_END;

void pmm_init(void *fdt);
void *pmm_alloc(usize *n);
void pmm_free(void *p, usize n);

#endif

#ifndef _PLIC_H
#define _PLIC_H

#include "stdint.h"

void plic_init(void *fdt);
void plic_enable(u32 id, usize hart);
void plic_set_priority(u32 id, u8 priority);
void plic_set_threshold(u8 threshold, usize hart);
u32 plic_next(usize hart);
void plic_claim(usize hart, u32 id);
void plic_start_hart(usize hart);

#endif

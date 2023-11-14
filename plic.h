#ifndef _PLIC_H
#define _PLIC_H

#include <stdint.h>

#define PLIC_PRIORITY ((volatile uint32_t *)0xc000000)
#define PLIC_PENDING ((volatile uint32_t *)0xc001000)
#define PLIC_ENABLE ((volatile uint32_t *)0xc002000)
#define PLIC_THRESHOLD ((volatile uint32_t *)0xc200000)
#define PLIC_CLAIM ((volatile uint32_t *)0xc200004)

#define PLIC_UART 10

void plic_enable(uint32_t id);
void plic_complete(uint32_t id);
void plic_set_priority(uint32_t id, uint8_t priority);
void plic_set_threshold(uint8_t threshold);
uint32_t plic_next(void);
void plic_init(void);

#endif

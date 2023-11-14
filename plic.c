#include "plic.h"

void plic_enable(uint32_t id)
{
    *PLIC_ENABLE |= 1 << id;
}

void plic_set_priority(uint32_t id, uint8_t priority)
{
    PLIC_PRIORITY[id] = priority & 7;
}

void plic_set_threshold(uint8_t threshold)
{
    *PLIC_THRESHOLD = threshold & 7;
}

uint32_t plic_next(void)
{
    return *PLIC_CLAIM;
}

void plic_complete(uint32_t id)
{
    *PLIC_CLAIM = id;
}

void plic_init(void)
{
    plic_set_threshold(0);
    plic_enable(PLIC_UART);
    plic_set_priority(PLIC_UART, 1);
}

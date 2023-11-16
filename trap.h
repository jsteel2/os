#ifndef _TRAP_H
#define _TRAP_H

#include <stdint.h>
#include <stddef.h>
#include "page.h"

#define MTIME ((uint64_t *)0x200bff8)
#define MTIMECMP ((uint64_t *)0x2004000)

typedef struct __attribute__((aligned(PAGE_SIZE))) __attribute__((packed))
{
    size_t regs[32];
    size_t fregs[32];
    size_t satp;
    uint64_t pc;
    size_t mhartid;
} Frame;

void clint_set(void);
size_t m_trap(size_t mepc, size_t mtval, size_t mcause, size_t mhart, size_t mstatus, Frame *frame);

#endif

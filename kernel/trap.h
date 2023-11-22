#ifndef _TRAP_H
#define _TRAP_H

#include "stdint.h"
#include "pmm.h"

typedef struct __attribute__((packed))
{
    usize regs[32];
    usize fregs[32];
    usize hartid;
    usize sbi_save[14];
} TrapFrame;

extern void trap_vector(void);
extern void enable_interrupts(void);

#endif

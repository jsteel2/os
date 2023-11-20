#ifndef _TRAP_H
#define _TRAP_H

#include "stdint.h"
#include "pmm.h"

typedef struct __attribute__((packed))
{
    usize regs[32];
    usize fregs[32];
    usize hartid;
    usize sbi_save[13];
} TrapFrame;

extern void trap_vector(void);

#endif

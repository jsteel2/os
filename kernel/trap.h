#ifndef _TRAP_H
#define _TRAP_H

#include "stdint.h"
#include "pmm.h"

// right now we dont actually need the alignment here, cuz we just pop this guy on the stack in trap.S
// but thatll change when we add multicore
typedef struct __attribute__((aligned(PAGE_SIZE))) __attribute__((packed))
{
    usize regs[32];
    usize fregs[32];
    usize satp;
    usize pc;
    usize mhartid;
} TrapFrame;

extern void trap_vector(void);

#endif

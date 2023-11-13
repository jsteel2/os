#include "trap.h"
#include "kprint.h"
#include <stdbool.h>

void clint_set(void)
{
    (*(uint64_t *)0x2004000) = (*(uint64_t *)0x200bff8) + 10000000;
}

size_t m_trap(size_t mepc, size_t mtval, size_t mcause, size_t mhart, size_t mstatus, Frame *frame)
{
    bool interrupt = mcause >> 63;
    int mcause_num = mcause & 0xffff;

    if (interrupt)
    {
        switch (mcause_num)
        {
            case 3: kprintf("Machine software interrupt\r\n"); break;
            case 7: kprintf("Machine timer interrupt\r\n"); clint_set(); break;
            case 11: kprintf("Machine external interrupt\r\n"); break;
            default: kprintf("PANIC: Unhandled interrupt %d\r\n", mcause_num); for (;;);
        }
    }
    else
    {
        switch (mcause_num)
        {
            case 2: kprintf("PANIC: Illegal Instruction\r\n"); for (;;);
            case 8: kprintf("E-Call from User Mode\r\n"); return mepc + 4;
            case 9: kprintf("E-Call from Supervisor Mode\r\n"); return mepc + 4;
            case 11: kprintf("PANIC: E-Call from Machine Mode\r\n"); for (;;);
            case 12: kprintf("PANIC: Instruction Page Fault\r\n"); for (;;);
            case 13: kprintf("PANIC: Load Page Fault\r\n"); for (;;);
            case 15: kprintf("PANIC: Store Page Fault\r\n"); for (;;);
            default: kprintf("PANIC: Unhandled exception\r\n"); for (;;);
        }
    }

    return mepc;
}

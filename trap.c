#include "trap.h"
#include "kprint.h"
#include "plic.h"
#include "uart.h"
#include "process.h"
#include <stdbool.h>

void clint_set(void)
{
    (*(uint64_t *)0x2004000) = (*(uint64_t *)0x200bff8) + 10000000;
}

size_t m_trap(size_t mepc, size_t mtval, size_t mcause, size_t mhart, size_t mstatus, Frame *frame)
{
    bool interrupt = mcause >> 63;
    int mcause_num = mcause & 0xfff;

    if (interrupt)
    {
        switch (mcause_num)
        {
            case 3: kprintf("Machine software interrupt\r\n"); break;
            case 7: kprintf("Machine timer interrupt\r\n"); clint_set(); schedule(); break;
            case 11:
            {
                uint32_t id = plic_next();
                switch (id)
                {
                    case 10: kprintf("%c", uart_read()); plic_complete(id); break;
                    default: kprintf("PANIC: Unhanded Machine External interrupt %d\r\n", id); for (;;);
                }
                break;
            }
            default: kprintf("PANIC: Unhandled interrupt %d\r\n", mcause_num); for (;;);
        }
    }
    else
    {
        switch (mcause_num)
        {
            case 2: kprintf("PANIC: Illegal Instruction\r\n"); for (;;);
            case 8: kprintf("E-Call from User Mode\r\n"); return mepc + 4;
            case 9: kprintf("E-Call from Supervisor Mode, Starting up!\r\n"); schedule(); break;
            case 11: kprintf("PANIC: E-Call from Machine Mode\r\n"); for (;;);
            case 12: kprintf("PANIC: Instruction Page Fault at 0x%x\r\n", mtval); for (;;);
            case 13: kprintf("PANIC: Load Page Fault at 0x%x\r\n", mtval); for (;;);
            case 15: kprintf("PANIC: Store Page Fault at 0x%x\r\n", mtval); for (;;);
            default: kprintf("PANIC: Unhandled exception 0x%x\r\n", mcause_num); for (;;);
        }
    }

    return mepc;
}

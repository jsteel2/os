#include "trap.h"
#include "kprint.h"
#include "plic.h"
#include "uart.h"
#include "process.h"
#include "syscall.h"
#include <stdbool.h>

void clint_set(void)
{
    *MTIMECMP = *MTIME + 50000;
}

void wfi(void)
{
    asm volatile("wfi");
}

void m_defer(uint64_t fn)
{
    // jesus christ
    *(ProcessMode *)virt_to_phys(&kernel_table, (size_t)&cur_process->mode) = PROCESS_SUPERVISOR;
    uint64_t *pc = (uint64_t *)virt_to_phys(&kernel_table, (size_t)&cur_process->sys_frame.pc);
    *(uint64_t *)virt_to_phys(&kernel_table, (size_t)&cur_process->sys_frame.regs[1]) = *pc;
    *pc = fn;
    uint64_t *stack = (uint64_t *)virt_to_phys(&kernel_table, (size_t)&cur_process->sys_stack);
    *(uint64_t *)virt_to_phys(&kernel_table, (size_t)&cur_process->sys_frame.regs[2]) = *stack + 510; // umm hardcode nto right uhh yeah
    run_process(cur_process);
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
            case 7: clint_set(); schedule(); return (size_t)wfi;
            case 11:
            {
                uint32_t id = plic_next();
                switch (id)
                {
                    case 10: kprintf("%c", uart_read()); plic_complete(id); break; // why does this give us E-Call from supervisor mode
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
            case 8: kprintf("E-Call from User Mode\r\n"), m_defer((uint64_t)syscall); break;
            case 9: kprintf("E-Call from Supervisor Mode\r\n"); if (frame->regs[10] == 0) schedule(); else run_process(cur_process); return (size_t)wfi;
            case 11: kprintf("PANIC: E-Call from Machine Mode\r\n"); for (;;);
            case 12: kprintf("PANIC: Instruction Page Fault at 0x%x\r\n", mtval); for (;;);
            case 13: kprintf("PANIC: Load Page Fault at 0x%x\r\n", mtval); for (;;);
            case 15: kprintf("PANIC: Store Page Fault at 0x%x\r\n", mtval); for (;;);
            default: kprintf("PANIC: Unhandled exception 0x%x\r\n", mcause_num); for (;;);
        }
    }

    return mepc;
}

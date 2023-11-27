#include "trap.h"
#include "sbi.h"
#include "kprint.h"
#include "time.h"
#include "plic.h"
#include "uart.h"

usize trap(usize epc, usize tval, usize cause, TrapFrame *frame)
{
    bool interrupt = cause >> 63;
    usize cause_num = cause & 0xfff;

    if (interrupt)
    {
        switch (cause_num)
        {
            case 5: kprintf("Supervisor timer interrupt from hart id %d", frame->hartid); sbi_call(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, time_read() + timebase_frequency, 0, 0); break;
            case 9:
            {
                u32 id = plic_next(frame->hartid);
                if (!id) break;
                switch (id)
                {
                    case 10: kprintf("%c from hart id %d", *uart, frame->hartid); break;
                    default: kprintf("Unhandled PLIC Interrupt %d from hart id %d", id, frame->hartid); break;
                }
                plic_claim(frame->hartid, id);
                break;
            }
            default: kprintf("Unhandled interrupt %d from hart id %d", cause_num, frame->hartid); break;
        }
    }
    else
    {
        kprintf("DEATH");
        for (;;);
    }
    return epc;
}

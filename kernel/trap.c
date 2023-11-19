#include "trap.h"
#include "sbi.h"
#include "kprint.h"
#include "time.h"

usize trap(usize epc, usize tval, usize cause, usize hart, usize status, TrapFrame *frame)
{
    kprintf("trap!");
    if (cause < 100)
    {
        kprintf("death");
        for (;;);
    }
    sbi_call(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, time_read() + timebase_frequency, 0, 0);
    return epc;
}

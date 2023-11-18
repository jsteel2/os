#include "trap.h"
#include "sbi.h"
#include "kprint.h"

usize trap(usize epc, usize tval, usize cause, usize hart, usize status, TrapFrame *frame)
{
    kprintf("trap!\r\n");
    if (cause < 100)
    {
        kprintf("death\r\n");
        for (;;);
    }
    sbi_call(SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER, -1);
    return epc;
}

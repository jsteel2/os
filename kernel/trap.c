#include "trap.h"
#include "sbi.h"

usize trap(usize epc, usize tval, usize cause, usize hart, usize status, TrapFrame *frame)
{
    if (cause < 100) for(;;);
    sbi_call(1, 0, 'g');
    sbi_call(0x54494D45, 0, -1);
    return epc;
}

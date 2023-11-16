#include "syscall.h"
#include "kprint.h"
#include "virt.h"
#include "process.h"

void exit(int code)
{
    if (cur_process->pid == 1)
    {
        kprintf("PANIC: Attempted to kill init!\r\n");
        // this doesnt actually panic anymore because we're in a supervisor mode task now
        // FIXME
        for (;;);
    }

    // how do you like, make sure that this doesnt fuck shit if we switch in the middle of it
    remove_process(cur_process);
}

void sleep(int seconds)
{
    cur_process->timecmp = *MTIME + seconds * 10000000;
    cur_process->state = PROCESS_SLEEPING;
}

void syscall(void)
{
    Frame frame = cur_process->frame;
    PageTable *table = (PageTable *)((frame.satp & (((size_t)1 << 44) - 1)) << 12);
    switch (frame.regs[10])
    {
        // this is a placeholder
        case SYS_WRITE: kprintf("0x%x: %s", frame.regs[11], virt_to_phys(table, frame.regs[11])); break;
        case SYS_EXIT: exit(frame.regs[11]); break;
        // this is also a placeholder, instead implement linux's nanosleep(2)
        case SYS_SLEEP: sleep(frame.regs[11]); break;
        default: kprintf("Invalid system call %d\r\n", frame.regs[10]); break;
    }

    cur_process->frame.pc += 4;
    cur_process->mode = PROCESS_USER;
    if (cur_process->state == PROCESS_RUNNING) asm volatile("li a0, 1\n\tecall");
    asm volatile("li a0, 0\n\tecall");
    asm volatile("wfi");
}

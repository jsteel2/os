#include "process.h"
#include "kmem.h"

Process *cur_process = NULL;

void add_process(Process *proc)
{
    if (!cur_process)
    {
        cur_process = proc;
        return;
    }

    proc->next = cur_process->next;
    proc->prev = cur_process;
    cur_process->next = proc;
}

void remove_process(Process *proc)
{
    proc->prev->next = proc->next;
    proc->next->prev = proc->prev;
    free(proc);
}

void run_process(Process *proc)
{
    if (proc->mode == PROCESS_USER)
    {
        uint64_t satp = ((size_t)8 << 60) | ((size_t)proc->pid << 44) | ((size_t)proc->page_table >> 12);
        asm_switch_to_user((uint64_t)&proc->frame, proc->frame.pc, satp);
    }
    else
    {
        uint64_t satp = ((size_t)8 << 60) | ((size_t)&kernel_table >> 12);
        asm_switch_to_supervisor((uint64_t)&proc->sys_frame, proc->sys_frame.pc, satp);
    }
}
void schedule(void)
{
    cur_process = cur_process->next;

    Process *begin = cur_process;

    while (cur_process->state != PROCESS_RUNNING)
    {
        if (cur_process->state == PROCESS_SLEEPING && cur_process->timecmp < *MTIME)
        {
            cur_process->state = PROCESS_RUNNING;
            break;
        }
        cur_process = cur_process->next;
        if (cur_process == begin) return;
    }

    run_process(cur_process);
}

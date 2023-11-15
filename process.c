#include "process.h"

Process *cur_process = NULL;

void add_process(Process *proc)
{
    if (!cur_process)
    {
        cur_process = proc;
        return;
    }

    proc->next = cur_process->next;
    cur_process->next = proc;
}

void schedule(void)
{
    cur_process = cur_process->next;

    uint64_t satp = ((size_t)8 << 60) | ((size_t)cur_process->pid << 44) | ((size_t)cur_process->page_table >> 12);

    asm_switch_to_user((uint64_t)&cur_process->frame, cur_process->pc, satp);
}

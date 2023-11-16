#ifndef _PROCESS_H
#define _PROCESS_H

#include "trap.h"
#include "virt.h"

#define STACK_ADDR 0xf000 /* change me */

typedef enum
{
    PROCESS_RUNNING,
    PROCESS_SLEEPING
} ProcessState;

typedef enum
{
    PROCESS_USER,
    PROCESS_SUPERVISOR
} ProcessMode;

struct _process
{
    Frame frame;
    Frame sys_frame;
    uint8_t *stack;
    uint8_t *sys_stack;
    uint16_t pid;
    PageTable *page_table;
    ProcessState state;
    ProcessMode mode;
    size_t timecmp;
    struct _process *next;
    struct _process *prev;
};

typedef struct _process Process;

void add_process(Process *proc);
void remove_process(Process *proc);
void run_process(Process *proc);
void schedule(void);

extern void asm_switch_to_user(uint64_t frame_addr, uint64_t pc, uint64_t satp);
extern void asm_switch_to_supervisor(uint64_t frame_addr, uint64_t pc, uint64_t satp);

extern Process *cur_process;

#endif

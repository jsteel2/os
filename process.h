#ifndef _PROCESS_H
#define _PROCESS_H

#include "trap.h"
#include "virt.h"

#define STACK_ADDR 0xf000 /* change me */

typedef enum
{
    PROCESS_RUNNING
} ProcessState;

struct _process
{
    Frame frame;
    uint8_t *stack;
    uint64_t pc;
    uint16_t pid;
    PageTable *page_table;
    ProcessState state;
    struct _process *next;
};

typedef struct _process Process;

void add_process(Process *proc);
void remove_process(Process *proc);
void schedule(void);

extern void asm_switch_to_user(uint64_t frame_addr, uint64_t pc, uint64_t satp);

#endif

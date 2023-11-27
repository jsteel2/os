#ifndef _VMM_H
#define _VMM_H

#include "pmm.h"

#define PAGE_1G_SIZE (PAGE_SIZE << 9 << 9)
#define PAGE_2M_SIZE (PAGE_SIZE << 9)

#define VMM_TABLE_ENTRIES 512

#define ENTRY_V (1 << 0)
#define ENTRY_R (1 << 1)
#define ENTRY_W (1 << 2)
#define ENTRY_X (1 << 3)
#define ENTRY_U (1 << 4)
#define ENTRY_G (1 << 5)
#define ENTRY_A (1 << 6)
#define ENTRY_D (1 << 7)

typedef enum
{
    PAGE_LEVEL_4K,
    PAGE_LEVEL_2M,
    PAGE_LEVEL_1G
} PageLevel;

typedef struct __attribute__((aligned(PAGE_SIZE))) __attribute__((packed))
{
    u64 entries[VMM_TABLE_ENTRIES];
} VMMTable;

void vmm_init(void);
void vmm_identity_map(VMMTable *table, usize start, usize len, u64 bits);
extern void vmm_enable(VMMTable *table);

extern VMMTable kernel_vmm_table;

#endif

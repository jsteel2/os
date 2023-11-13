#ifndef _VIRT_H
#define _VIRT_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_TABLE_ENTRIES 512

#define ENTRY_V (1 << 0)
#define ENTRY_R (1 << 1)
#define ENTRY_W (1 << 2)
#define ENTRY_X (1 << 3)
#define ENTRY_U (1 << 4)
#define ENTRY_G (1 << 5)
#define ENTRY_A (1 << 6)
#define ENTRY_D (1 << 7)
#define ENTRY_L (1 << 8)

typedef struct
{
    uint64_t entries[PAGE_TABLE_ENTRIES];
} PageTable;

typedef enum
{
    PAGE_4K,
    PAGE_2M,
    PAGE_1G,
    PAGE_GET = PAGE_4K
} PageSize;

extern size_t TEXT_START;
extern size_t TEXT_END;
extern size_t BSS_START;
extern size_t BSS_END;
extern size_t RODATA_START;
extern size_t RODATA_END;
extern size_t DATA_START;
extern size_t DATA_END;
extern PageTable kernel_table;

void virt_enable();
uint64_t *virt_page_get(PageTable *table, size_t vaddr, unsigned level, PageSize size);
uint8_t *virt_pages_alloc(PageTable *table, size_t pages, uint64_t bits);
void virt_pages_free(PageTable *table, uint8_t *vaddr);

extern void asm_virt_enable(uint64_t satp);

#endif

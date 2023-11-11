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

typedef struct __attribute__((packed))
{
    uint64_t entries[PAGE_TABLE_ENTRIES];
} PageTable;

extern size_t _text_start;
extern size_t _text_end;
extern size_t _bss_start;
extern size_t _bss_end;
extern size_t _rodata_start;
extern size_t _rodata_end;
extern size_t _data_start;
extern size_t _data_end;
extern PageTable kernel_table;

void virt_enable(PageTable *table);
uint8_t *virt_page_alloc(PageTable *table, size_t pages);
void virt_page_free(PageTable *table, uint8_t *p);

extern void asm_virt_enable(uint64_t satp);

#endif
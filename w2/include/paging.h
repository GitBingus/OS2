#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "kbool.h"

#define PAGE_SIZE       4096
#define PAGE_ENTRIES    1024

#define PAGE_PRESENT    0x001
#define PAGE_WRITABLE   0x002
#define PAGE_USER       0x004

#define IDENTITY_TABLE_COUNT 32

#define VM_ALLOC_BASE  0x08000000
#define VM_ALLOC_LIMIT 0x10000000
#define VM_PAGE_COUNT  ((VM_ALLOC_LIMIT - VM_ALLOC_BASE) / PAGE_SIZE)

void paging_init(void); // Initialize paging and enable it
bool paging_map(u32int virtual_address, u32int physical_address, u32int flags); // Map a virtual address to a physical address with specified flags
bool paging_unmap(u32int virtual_addr, u32int *physical_address); // Unmap a virtual address and optionally retrieve the physical address

void *vm_alloc_pages(u32int page_count, u32int flags); // Allocate a specified number of contiguous virtual pages with given flags
bool vm_free_pages(void *address, u32int page_count); // Free a specified number of contiguous virtual pages starting from the given address



#endif // PAGING_H

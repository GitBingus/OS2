#include "paging.h"
#include "types.h"
#include "pmm.h"

#define IDENTITY_TABLE_COUNT 32

static u32int page_directory[PAGE_ENTRIES]
    __attribute__((aligned(4096)));

static u32int page_tables[IDENTITY_TABLE_COUNT][PAGE_ENTRIES]
    __attribute__((aligned(4096)));


void paging_init(void)
{
    for (u32int i = 0; i < PAGE_ENTRIES; i++) {
        page_directory[i] = 0;
    }

    // Initialise page tables
    for (u32int i = 0; i < IDENTITY_TABLE_COUNT; i++) {
        for (u32int j = 0; j < PAGE_ENTRIES; j++) {
            u32int address = (i * PAGE_ENTRIES + j) * PAGE_SIZE;
            page_tables[i][j] = address | PAGE_PRESENT | PAGE_WRITABLE;
        }
    }

    // Add page tables to the directory
    for (u32int i = 0; i < IDENTITY_TABLE_COUNT; i++) {
        page_directory[i] = (u32int) &page_tables[i] | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Load CR3
    __asm__ volatile (
        "mov %0, %%cr3"
        :
        : "r"(page_directory)
        : "memory"
    );

    // Enable paging by setting the PG bit in CR0
    __asm__ volatile (
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t"
        "mov %%eax, %%cr0"
        :
        :
        : "eax", "memory"
    );
}

bool paging_map(
    u32int virtual_addr,
    u32int physical_addr,
    u32int flags
) {
    u32int pd_index = (virtual_addr >> 22) & 0x3FF; // directory index
    u32int pt_index = (virtual_addr >> 12) & 0x3FF; // table index

    if ((virtual_addr & 0xFFF) != 0 || (physical_addr & 0xFFF) != 0) return false;

    // Check if the page table is present
    if (!(page_directory[pd_index] & PAGE_PRESENT)) {
        u32int physical = pmm_alloc_frame();

        if (physical == 0) {
            return false; // Out of physical memory
        }

        u32int *new_page_table = (u32int *)physical;

        // Initialise the page table
        for (u32int i = 0; i < PAGE_ENTRIES; i++) {
            new_page_table[i] = 0;
        }

        // Register the page table in the directory
        page_directory[pd_index] = physical | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Map the physical address to the virtual address
    u32int *page_table = (u32int *)(page_directory[pd_index] & ~0xFFF); // check for an existing mapping
    if (page_table[pt_index] & PAGE_PRESENT) return false;

    page_table[pt_index] = (physical_addr & 0xFFFFF000) | flags | PAGE_PRESENT;

    __asm__ volatile (
        "invlpg (%0)"
        :
        : "r"(virtual_addr)
        : "memory"
    );

    return true;
}

bool paging_unmap(u32int virtual_addr) {
    if (virtual_addr & 0xFFF)
        return false;

    u32int pd_index = (virtual_addr >> 22) & 0x3FF;
    u32int pt_index = (virtual_addr >> 12) & 0x3FF;

    if (!(page_directory[pd_index] & PAGE_PRESENT))
        return false;

    u32int *page_table =
        (u32int *)(page_directory[pd_index] & 0xFFFFF000);

    if (!(page_table[pt_index] & PAGE_PRESENT))
        return false;

    page_table[pt_index] = 0;

    __asm__ volatile (
        "invlpg (%0)"
        :
        : "r"(virtual_addr)
        : "memory"
    );

    return true;
}
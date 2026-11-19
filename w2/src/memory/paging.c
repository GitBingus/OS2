#include "paging.h"
#include "types.h"
#include "pmm.h"
#include "kbool.h"

#define IDENTITY_TABLE_COUNT 32

static u32int page_directory[PAGE_ENTRIES]
    __attribute__((aligned(4096)));

static u32int page_tables[IDENTITY_TABLE_COUNT][PAGE_ENTRIES]
    __attribute__((aligned(4096)));


static u8int virtual_bitmap[(VM_PAGE_COUNT + 7) / 8];

static bool vm_page_used(u32int page);
static void vm_mark_page_used(u32int page);
static void vm_mark_page_free(u32int page);
static s32int vm_find_free_run(u32int page_count);


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

bool paging_unmap(u32int virtual_addr, u32int *physical_addr) {
    if (virtual_addr & 0xFFF)
        return false;

    u32int pd_index = (virtual_addr >> 22) & 0x3FF;
    u32int pt_index = (virtual_addr >> 12) & 0x3FF;

    if (!(page_directory[pd_index] & PAGE_PRESENT))
        return false;

    // Get the page table from the page directory
    
    u32int *page_table =
        (u32int *)(page_directory[pd_index] & 0xFFFFF000);

    if (!(page_table[pt_index] & PAGE_PRESENT))
        return false;

    // Optionally return the physical address of the unmapped page

    if (physical_addr) {
        *physical_addr = page_table[pt_index] & 0xFFFFF000; 
    }

    page_table[pt_index] = 0;

    __asm__ volatile ( // Invalidate the TLB entry for the unmapped virtual address
        "invlpg (%0)"
        :
        : "r"(virtual_addr)
        : "memory"
    );

    return true;
}

static void vm_mark_page_used(u32int page) {
    virtual_bitmap[page / 8] |= (1 << (page % 8)); // Mark the page as used in the bitmap
}

static void vm_mark_page_free(u32int page) {
    virtual_bitmap[page / 8] &= ~(1 << (page % 8)); // Mark the page as free in the bitmap
}

static bool vm_page_used(u32int page) {
    return (virtual_bitmap[page / 8] & (1 << (page % 8))) != 0; // Check if the page is marked as used in the bitmap
}

static s32int vm_find_free_run(u32int page_count) {
    u32int consecutive = 0;
    for (u32int i = 0; i < VM_PAGE_COUNT; i++) {
        if (!vm_page_used(i)) {
            consecutive++;
            if (consecutive == page_count) {
                return i - page_count + 1; // Return the starting index of the free run
            }
        } else {
            consecutive = 0; // Reset if a used page is found
        }
    }
    return -1; // No sufficient free run found
}

/*
    validate page_count
    find a free virtual run
    for each page:
        allocate a physical frame
        map it with paging_map()
        if anything fails:
            undo all previous mappings
            return those frames
    mark virtual pages used
    return VM_ALLOC_BASE + offset
*/

void *vm_alloc_pages(u32int page_count, u32int flags) {
    if (page_count == 0) return NULL;

    s32int offset = vm_find_free_run(page_count);
    if (offset < 0) return NULL;

    for (u32int i = 0; i < page_count; i++) {
        u32int physical = pmm_alloc_frame();
        if (physical == 0) {
            // Undo all previous allocations
            for (u32int j = 0; j < i; j++) {
                u32int virtual = VM_ALLOC_BASE + ((offset + j) * PAGE_SIZE);
                paging_unmap(virtual, &physical);
            }
            return NULL;
        }

        u32int virtual = VM_ALLOC_BASE + ((offset + i) * PAGE_SIZE);
        if (!paging_map(virtual, physical, flags)) {
            pmm_free_frame(physical);
            // Undo all previous allocations
            for (u32int j = 0; j < i; j++) {
                u32int virtual = VM_ALLOC_BASE + ((offset + j) * PAGE_SIZE);
                paging_unmap(virtual, &physical);
            }
            return NULL;
        }
    }

    for (u32int i = 0; i < page_count; i++) {
        vm_mark_page_used(offset + i);
    }

    return (void *)(VM_ALLOC_BASE + (offset * PAGE_SIZE));
}

bool vm_free_pages(void *address, u32int page_count) {
    if (address == 0 || page_count == 0)
        return false;

    // Validate that the address is within the virtual allocation range and is page-aligned
    u32int virtual_base = (u32int)address;

    if (virtual_base < VM_ALLOC_BASE ||
        virtual_base >= VM_ALLOC_LIMIT)
        return false;

    if ((virtual_base - VM_ALLOC_BASE) % PAGE_SIZE != 0)
        return false;

    u32int first_page =
        (virtual_base - VM_ALLOC_BASE) / PAGE_SIZE;

    if (first_page >= VM_PAGE_COUNT ||
        page_count > VM_PAGE_COUNT - first_page)
        return false;

    // Check that all pages are currently allocated
    for (u32int i = 0; i < page_count; i++) {
        u32int virtual_address =
            virtual_base + (i * PAGE_SIZE);

        u32int physical_address;

        if (!paging_unmap(
                virtual_address,
                &physical_address)) {
            return false;
        }

        if (!pmm_free_frame(physical_address)) {
            return false;
        }

        vm_mark_page_free(
            (virtual_address - VM_ALLOC_BASE) / PAGE_SIZE
        );
    }

    return true;
}
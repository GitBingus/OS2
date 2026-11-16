#include "pmm.h"
#include "types.h"
#include "kstring.h"
#include "tui.h"

#define PMM_FRAME_SIZE 4096
#define PMM_MAX_MEMORY (128 * 1024 * 1024)
#define PMM_MAX_FRAMES (PMM_MAX_MEMORY / PMM_FRAME_SIZE)

static u8int pmm_bitmap[PMM_MAX_FRAMES / 8];

static void pmm_set_frame(u32int frame);
static void pmm_clear_frame(u32int frame);
static bool pmm_test_frame(u32int frame);
static void pmm_mark_region_used(u64int base, u64int length);
u32int pmm_get_free_frames(void);
u32int pmm_alloc_frame(void);

static bool pmm_initialized = false;

extern char kernel_start;
extern char kernel_end;

static void pmm_set_frame(u32int frame) {
    if (frame >= PMM_MAX_FRAMES)
        return;

    u32int byte_index = frame / 8;
    u32int bit_index = frame % 8;

    pmm_bitmap[byte_index] |= (u8int)(1u << bit_index); // It sets the selected bit to 1 without changing any other bits
}

u32int pmm_get_free_frames(void) {
    u32int count = 0;

    for (u32int i = 0; i < PMM_MAX_FRAMES; i++) {
        if (!pmm_test_frame(i)) {
            count++;
        }
    }

    return count;
}

static void pmm_clear_frame(u32int frame) {
    if (frame >= PMM_MAX_FRAMES)
        return;

    u32int byte_index = frame / 8;
    u32int bit_index = frame % 8;

    pmm_bitmap[byte_index] &= (u8int)~(1u << bit_index); // This function clears a bit to 0
}

static bool pmm_test_frame(u32int frame) {
    if (frame >= PMM_MAX_FRAMES)
        return true; // Treat unsupported frames as unavailable.

    u32int byte_index = frame / 8;
    u32int bit_index = frame % 8;

    return (pmm_bitmap[byte_index] & (1u << bit_index)) != 0;
}

static void pmm_mark_region_used(u64int base, u64int length) {
    // Ignore empty or completely unsupported regions
    if (length == 0 || base >= PMM_MAX_MEMORY)
        return;

    u64int end;

    if (length > (u64int)-1 - base)
        end = (u64int)-1;
    else
        end = base + length;

    // Clip to our supported physical memory range
    if (end > PMM_MAX_MEMORY)
        end = PMM_MAX_MEMORY;

    u64int first_frame = base / PMM_FRAME_SIZE;
    u64int end_frame = end / PMM_FRAME_SIZE;

    if (end % PMM_FRAME_SIZE != 0)
        end_frame++;

    for (u64int i = first_frame; i < end_frame; i++) {
        pmm_set_frame((u32int)i);
    }
}

void uint64_to_hex(u64int value, char *buffer) {
    char hexChars[] = "0123456789ABCDEF";

    buffer[0] = '0';
    buffer[1] = 'x';
    
    for (int i = 15; i >= 0; i--) {
        int nibble = value & 0xF;

        buffer[i + 2] = hexChars[nibble];

        value >>= 4;
    }

    buffer[18] = '\0';
}

static void pmm_mark_region_free(u64int base, u64int length) {
    // Ignore empty or completely unsupported regions
    if (length == 0 || base >= PMM_MAX_MEMORY)
        return;

    u64int end;

    if (length > (u64int)-1 - base)
        end = (u64int)-1;
    else
        end = base + length;

    // Clip to our supported physical memory range
    if (end > PMM_MAX_MEMORY)
        end = PMM_MAX_MEMORY;

    u64int first_frame = base / PMM_FRAME_SIZE;

    if (base % PMM_FRAME_SIZE != 0)
        first_frame++;

    u64int end_frame = end / PMM_FRAME_SIZE;

    for (u64int i = first_frame; i < end_frame; i++) {
        pmm_clear_frame((u32int)i);
    }
}

u32int pmm_alloc_frame(void) {
    if (!pmm_initialized) return 0;
    for (u32int i = 0; i < PMM_MAX_FRAMES; i++) {
        if (!pmm_test_frame(i)) {
            pmm_set_frame(i);
            return i * PMM_FRAME_SIZE;
        }
    }
    return 0; // No free frames available
}

void pmm_init(multiboot_info_t *mb_info) {
    if (mb_info == 0)
        return;

    if (!(mb_info->flags & MULTIBOOT_INFO_MEM_MAP))
        return;
    
    // Initialize the page frame bitmap
    for (u32int i = 0; i < sizeof(pmm_bitmap); i++) { // makes every frame unavailable initially
        pmm_bitmap[i] = 0xFF;
    }

    u32int current = mb_info->mmap_addr;
    u32int remaining = mb_info->mmap_length;

    while (remaining > 0) {

        // 1. Ensure at least 4 bytes remain for the size field
        if (remaining < sizeof(u32int))
            break;

        // 2. Interpret the current address as a memory map entry
        multiboot_memory_map_t *entry =
            (multiboot_memory_map_t *)current;

        // 3. Validate the size of this entry
        if (entry->size < 20) break;
        if (entry->size > remaining - sizeof(u32int)) break;

        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            pmm_mark_region_free(entry->addr, entry->len);
        }

        cstring status = "";

        // 4. Inspect the memory region
        switch (entry->type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                status = ("Available");
                break;

            case MULTIBOOT_MEMORY_RESERVED:
                status = ("Reserved");
                break;

            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                status = ("ACPI Reclaimable");
                break;

            case MULTIBOOT_MEMORY_NVS:
                status = ("ACPI NVS");
                break;

            case MULTIBOOT_MEMORY_BADRAM:
                status = ("Bad RAM");
                break;

            default:
                status = ("Unknown");
                break;
        }

        tui_write("Type: ");
        tui_write(status);
        tui_newline();

        char addr_str[19];
        char len_str[19];

        uint64_to_hex(entry->addr, addr_str);
        uint64_to_hex(entry->len, len_str);

        tui_write("Base: ");
        tui_write(addr_str);
        tui_newline();

        tui_write("Length: ");
        tui_write(len_str);
        tui_newline();

        // 5. Calculate the total size of this entry
        u32int entry_size = entry->size + sizeof(u32int);

        // 6. Move to the next entry
        current += entry_size;
        remaining -= entry_size;
    }

    u32int start = (u32int)&kernel_start;
    u32int end = (u32int)&kernel_end;
    
    pmm_mark_region_used(start, end - start);
    // Mark the memory regions as used based on the multiboot memory map
    pmm_mark_region_used(mb_info->mmap_addr, mb_info->mmap_length);
    pmm_mark_region_used(
        (u32int)mb_info,
        sizeof(multiboot_info_t)
    );
    // Reserve the first 1MB of memory
    pmm_mark_region_used(0, 0x100000);
    // Reserve low memory and any Multiboot data or modules
    if (mb_info->flags & MULTIBOOT_INFO_MODS) {
        multiboot_module_t *mods =
            (multiboot_module_t *)mb_info->mods_addr;
        pmm_mark_region_used(
            mb_info->mods_addr,
            mb_info->mods_count * sizeof(multiboot_module_t)
        );
        for (u32int i = 0; i < mb_info->mods_count; i++) {
            if (mods[i].mod_end > mods[i].mod_start) {
                pmm_mark_region_used(
                    mods[i].mod_start,
                    mods[i].mod_end - mods[i].mod_start
                );
            }
        }
    }
    // pmm_set_frame(256);                         // Mark used.
 
    // bool test1 = pmm_test_frame(256);
    // char test1BUF[32];                          // Should return true

    // uint_to_string((int)test1, test1BUF);

    // pmm_clear_frame(256);                       // Mark free.

    // bool test2 = pmm_test_frame(256);           // Should return false
    // char test2BUF[32];
 
    // uint_to_string((int)test2, test2BUF);

    // tui_newline();
    // 
    // tui_write(test1BUF);
    // tui_newline();
    // 
    // tui_write(test2BUF);
    // tui_newline();
    
    pmm_initialized = true;
}
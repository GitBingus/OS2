#include "pmm.h"
#include "types.h"
#include "kstring.h"
#include "tui.h"

void pmm_init(multiboot_info_t *mb_info) {
    if (mb_info == 0)
        return;

    if (!(mb_info->flags & MULTIBOOT_INFO_MEM_MAP))
        return;

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
        // TODO: Check entry->size is at least 20 bytes
        // TODO: Check the complete entry fits within remaining

        if (entry->size < 20) break;
        if (entry->size > remaining - sizeof(u32int)) break;

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

        tui_write(status);

        // 5. Calculate the total size of this entry
        u32int entry_size = entry->size + sizeof(u32int);

        // 6. Move to the next entry
        current += entry_size;
        remaining -= entry_size;
    }
}
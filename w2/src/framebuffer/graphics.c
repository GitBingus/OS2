#include "graphics.h"

static graphics_framebuffer_t framebuffer;

void graphics_init(multiboot_info_t *mb_info) {
    /*
        Check that GRUB supplied framebuffer information
        Retrieve the framebuffer address
        Retrieve the actual width, height, pitch and colour depth
        Verify that the framebuffer format is supported
        Make the framebuffer memory accessible through your paging system
        Store the information for other graphics functions to use
    */

    // Validate Multiboot information
    if (mb_info == 0 || (!(mb_info->flags & (1 << 12)))) {
        return;
    }

    //  Retrieve framebuffer information from Multiboot structure

    u64int  framebuffer_addr    = mb_info->framebuffer_addr;
    u32int  framebuffer_pitch   = mb_info->framebuffer_pitch; 
    u32int  framebuffer_width   = mb_info->framebuffer_width; 
    u32int  framebuffer_height  = mb_info->framebuffer_height;
    u8int   framebuffer_bpp     = mb_info->framebuffer_bpp;
    u8int   framebuffer_type    = mb_info->framebuffer_type;
    u64int  framebuffer_size    = (u64int)framebuffer_pitch * framebuffer_height;

    if (framebuffer_addr > 0xFFFFFFFFULL) return; // Framebuffer address exceeds 32-bit limit

    if (framebuffer_size == 0 ||
        framebuffer_size > 0x100000000ULL - framebuffer_addr) {
        return; // Framebuffer size is zero or exceeds 32-bit limit
    }

    if (framebuffer_width == 0 || framebuffer_height == 0) return; // Framebuffer dimensions are invalid
    if (framebuffer_pitch == 0) return; // Framebuffer pitch is invalid

    u64int page_start = framebuffer_addr & ~(u64int)(PAGE_SIZE - 1);
    u64int framebuffer_end = framebuffer_addr + framebuffer_size;
    if (framebuffer_end > 0x100000000ULL - (PAGE_SIZE - 1)) return; // Framebuffer end address exceeds 32-bit limit
    if (mb_info->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) { // Check if framebuffer info is available
        u64int framebuffer_size = 
            (u64int)mb_info->framebuffer_pitch *
            mb_info->framebuffer_height;

        // Mark the framebuffer region as used in the physical memory manager
        pmm_mark_region_used(
            mb_info->framebuffer_addr,
            framebuffer_size
        );
    }
    
    u64int page_end = (framebuffer_end + PAGE_SIZE - 1) & ~(u64int)(PAGE_SIZE - 1);
    u32int mapped_virtual_address = (u32int)framebuffer_addr;

    // Map the framebuffer memory to the same virtual address as its physical address
    for (u64int page = page_start; page < page_end; page += PAGE_SIZE) {
        if (!paging_map(
            (u32int)page,
            (u32int)page,
            PAGE_PRESENT | PAGE_WRITABLE
        )) {
            return;
        }
    }

    // Store the framebuffer information for other graphics functions to use
    framebuffer.physical_address = framebuffer_addr;
    framebuffer.pixels = (volatile u8int *)mapped_virtual_address;

    framebuffer.pitch = framebuffer_pitch;
    framebuffer.width = framebuffer_width;
    framebuffer.height = framebuffer_height;

    framebuffer.bpp = framebuffer_bpp;
    framebuffer.type = framebuffer_type;
}
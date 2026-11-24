#include "graphics.h"

static graphics_framebuffer_t framebuffer;

bool graphics_init(multiboot_info_t *mb_info) {
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
        return false;
    }

    //  Retrieve framebuffer information from Multiboot structure

    u64int  framebuffer_addr    = mb_info->framebuffer_addr;
    u32int  framebuffer_pitch   = mb_info->framebuffer_pitch; 
    u32int  framebuffer_width   = mb_info->framebuffer_width; 
    u32int  framebuffer_height  = mb_info->framebuffer_height;
    u8int   framebuffer_bpp     = mb_info->framebuffer_bpp;
    u8int   framebuffer_type    = mb_info->framebuffer_type;
    u64int  framebuffer_size    = (u64int)framebuffer_pitch * framebuffer_height;

    if (framebuffer_bpp != 32) return false; // Only support 32 bits per pixel (RGB)
    if (framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) return false; // Only support RGB
    if (framebuffer_pitch < framebuffer_width * 4) return false; // Invalid pitch (should be at least as wide as the framebuffer width)

    if (framebuffer_addr > 0xFFFFFFFFULL) return false; // Framebuffer address exceeds 32-bit limit

    if (framebuffer_size == 0 ||
        framebuffer_size > 0x100000000ULL - framebuffer_addr) {
        return false; // Framebuffer size is zero or exceeds 32-bit limit
    }

    if (framebuffer_width == 0 || framebuffer_height == 0) return false; // Framebuffer dimensions are invalid
    if (framebuffer_pitch == 0) return false; // Framebuffer pitch is invalid

    u64int page_start = framebuffer_addr & ~(u64int)(PAGE_SIZE - 1);
    u64int framebuffer_end = framebuffer_addr + framebuffer_size;
    if (framebuffer_end > 0x100000000ULL - (PAGE_SIZE - 1)) return false; // Framebuffer end address exceeds 32-bit limit
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
            return false;
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
    framebuffer.red_position = mb_info->framebuffer_red_field_position;
    framebuffer.red_mask_size = mb_info->framebuffer_red_mask_size;

    framebuffer.green_position = mb_info->framebuffer_green_field_position;
    framebuffer.green_mask_size = mb_info->framebuffer_green_mask_size;

    framebuffer.blue_position = mb_info->framebuffer_blue_field_position;
    framebuffer.blue_mask_size = mb_info->framebuffer_blue_mask_size;

    return true;
}

void graphics_clear(u8int red, u8int green, u8int blue) {
    for (u32int y = 0; y < framebuffer.height; y++) {
        for (u32int x = 0; x < framebuffer.width; x++) {
            graphics_putpixel(x, y, red, green, blue);
        }
    }
}

void graphics_putpixel(
    u32int x,
    u32int y,
    u8int red,
    u8int green,
    u8int blue
) {
    if (framebuffer.pixels == 0) return;
    if (x >= framebuffer.width || y >= framebuffer.height) return;

    u32int offset = (y * framebuffer.pitch) + (x * 4);

    u32int pixel = // Combine the RGB values into a single pixel value.
        ((u32int)red   << framebuffer.red_position)   |
        ((u32int)green << framebuffer.green_position) |
        ((u32int)blue  << framebuffer.blue_position);

    volatile u32int *pixel_address =
        (volatile u32int *)(framebuffer.pixels + offset);

    *pixel_address = pixel;
}


void graphics_rgb_test(void) {
    static const u8int levels[6] = {
        0, 51, 102, 153, 204, 255
    };

    if (framebuffer.pixels == 0)
        return;

    graphics_clear(0, 0, 0);

    const u32int columns = 18;
    const u32int rows = 12;

    for (u32int i = 0; i < 216; i++) {
        u32int red_index   = i % 6;
        u32int green_index = (i / 6) % 6;
        u32int blue_index  = i / 36;

        u8int red   = levels[red_index];
        u8int green = levels[green_index];
        u8int blue  = levels[blue_index];

        u32int column = i % columns;
        u32int row    = i / columns;

        // Calculate cell boundaries without leaving gaps.
        u32int x1 = column * framebuffer.width / columns;
        u32int x2 = (column + 1) * framebuffer.width / columns;

        u32int y1 = row * framebuffer.height / rows;
        u32int y2 = (row + 1) * framebuffer.height / rows;

        graphics_draw_rect(
            x1,
            y1,
            x2 - x1,
            y2 - y1,
            red,
            green,
            blue
        );
    }
}


void graphics_draw_rect(
    u32int x,
    u32int y,
    u32int width,
    u32int height,
    u8int red,
    u8int green,
    u8int blue
) {
    if (x >= framebuffer.width || y >= framebuffer.height) return;

    if (width > framebuffer.width - x)
        width = framebuffer.width - x;

    if (height > framebuffer.height - y)
        height = framebuffer.height - y;

    for (u32int py = 0; py < height; py++) {
        for (u32int px = 0; px < width; px++) {
            graphics_putpixel(
                x + px, y + py, red, green, blue
            );
        }
    }
}
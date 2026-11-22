#include "io.h"
#include "fb.h"
#if FB_MODE
#include "paging.h"
#endif

const unsigned char foreground = 0x0F;
const unsigned char background = 0x00;
unsigned char attribute = FB_COLOUR(0x0F, 0x00);
framebuffer_t framebuffer;

const unsigned long screen_size = SCREEN_WIDTH * SCREEN_HEIGHT;

bool framebuffer_init(multiboot_info_t *mbi) {
    if (!(mbi->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO)) { // check if framebuffer info is available
        return false;
    }

    if (mbi -> framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) { // only support RGB framebuffer
        return false;
    }

    if (mbi->framebuffer_bpp != 32) { // only support 32 bits per pixel
        return false;
    }

    framebuffer.address = (u32int)mbi->framebuffer_addr;
    framebuffer.width = mbi->framebuffer_width;
    framebuffer.height = mbi->framebuffer_height;
    framebuffer.pitch = mbi->framebuffer_pitch;
    framebuffer.bpp = mbi->framebuffer_bpp;
    framebuffer.red_position = mbi->framebuffer_red_field_position;
    framebuffer.red_mask_size = mbi->framebuffer_red_mask_size;
    framebuffer.green_position = mbi->framebuffer_green_field_position;
    framebuffer.green_mask_size = mbi->framebuffer_green_mask_size;
    framebuffer.blue_position = mbi->framebuffer_blue_field_position;
    framebuffer.blue_mask_size = mbi->framebuffer_blue_mask_size;

    if (framebuffer.address != mbi->framebuffer_addr)
        return false;

    return true;
}

void framebuffer_put_pixel(u32int x, u32int y, u32int color) {
    if (x >= framebuffer.width || y >= framebuffer.height) {
        return; // out of bounds
    }

    u32int *row = (u32int *)((u8int *)framebuffer.address + y * framebuffer.pitch);
    row[x] = color;
}

u32int framebuffer_rgb(u8int red, u8int green, u8int blue) {
    u32int color = 0;

    color |= (red & ((1 << framebuffer.red_mask_size) - 1)) << framebuffer.red_position;
    color |= (green & ((1 << framebuffer.green_mask_size) - 1)) << framebuffer.green_position;
    color |= (blue & ((1 << framebuffer.blue_mask_size) - 1)) << framebuffer.blue_position;

    return color;
}

/* Write one character and its colour to a screen cell. */
void fb_write_cell(u32int index, char c, unsigned char fg, unsigned char bg) {
    u32int x = (index % SCREEN_WIDTH);
    u32int y = (index / SCREEN_WIDTH);
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
        return;
    unsigned char colour = FB_COLOUR(fg, bg);
    VGA_MEMORY[y * SCREEN_WIDTH + x] = ((unsigned short)colour << 8) | (unsigned char)c;
}

void fb_clear(unsigned char background) {
    unsigned char attribute =
        ((unsigned char)background << 4) | foreground;

    for (unsigned int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        VGA_MEMORY[i] = ((unsigned short)attribute << 8) | ' ';
    
    fb_move_cursor(0);
}


void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

void fb_scroll(void) {
    u32int row_bytes = framebuffer.pitch;
    u32int move_bytes = row_bytes * (SCREEN_HEIGHT - 1) * 16;

    u8int *destination = (u8int *)framebuffer.address;
    u8int *source = destination + row_bytes * 16;

    for (u32int i = 0; i < move_bytes; i++)
        destination[i] = source[i];

    u32int color = framebuffer_rgb(background * 17, background * 17, background * 17);
    u32int bottom_row = framebuffer.height >= 16 ? framebuffer.height - 16 : 0;

    for (u32int y = bottom_row; y < framebuffer.height; y++) {
        u32int *row = (u32int *)((u8int *)framebuffer.address + y * row_bytes);
        for (u32int x = 0; x < row_bytes / 4; x++)
            row[x] = color;
    }
}
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

#if FB_MODE
    u32int start = framebuffer.address & ~0xFFF;
    u32int end = (framebuffer.address + framebuffer.height * framebuffer.pitch + 0xFFF) & ~0xFFF;

    for (u32int address = start; address < end; address += PAGE_SIZE) {
        if (!paging_map(address, address, PAGE_PRESENT | PAGE_WRITABLE))
            return false;
    }
#endif

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

#if FB_MODE
static u8int font_row(char c, u8int row) {
    if (c >= 'a' && c <= 'z')
        c = (char)(c - 'a' + 'A');

    switch (c) {
        case 'A': { u8int p[] = {0x1C,0x22,0x22,0x3E,0x22,0x22,0x22}; return p[row]; }
        case 'B': { u8int p[] = {0x3C,0x22,0x22,0x3C,0x22,0x22,0x3C}; return p[row]; }
        case 'C': { u8int p[] = {0x1E,0x20,0x20,0x20,0x20,0x20,0x1E}; return p[row]; }
        case 'D': { u8int p[] = {0x3C,0x22,0x22,0x22,0x22,0x22,0x3C}; return p[row]; }
        case 'E': { u8int p[] = {0x3E,0x20,0x20,0x38,0x20,0x20,0x3E}; return p[row]; }
        case 'F': { u8int p[] = {0x3E,0x20,0x20,0x38,0x20,0x20,0x20}; return p[row]; }
        case 'G': { u8int p[] = {0x1E,0x20,0x20,0x26,0x22,0x22,0x1E}; return p[row]; }
        case 'H': { u8int p[] = {0x22,0x22,0x22,0x3E,0x22,0x22,0x22}; return p[row]; }
        case 'I': { u8int p[] = {0x3E,0x08,0x08,0x08,0x08,0x08,0x3E}; return p[row]; }
        case 'J': { u8int p[] = {0x02,0x02,0x02,0x02,0x22,0x22,0x1C}; return p[row]; }
        case 'K': { u8int p[] = {0x22,0x24,0x28,0x30,0x28,0x24,0x22}; return p[row]; }
        case 'L': { u8int p[] = {0x20,0x20,0x20,0x20,0x20,0x20,0x3E}; return p[row]; }
        case 'M': { u8int p[] = {0x22,0x36,0x2A,0x2A,0x22,0x22,0x22}; return p[row]; }
        case 'N': { u8int p[] = {0x22,0x32,0x32,0x2A,0x26,0x26,0x22}; return p[row]; }
        case 'O': { u8int p[] = {0x1C,0x22,0x22,0x22,0x22,0x22,0x1C}; return p[row]; }
        case 'P': { u8int p[] = {0x3C,0x22,0x22,0x3C,0x20,0x20,0x20}; return p[row]; }
        case 'Q': { u8int p[] = {0x1C,0x22,0x22,0x22,0x2A,0x24,0x1A}; return p[row]; }
        case 'R': { u8int p[] = {0x3C,0x22,0x22,0x3C,0x28,0x24,0x22}; return p[row]; }
        case 'S': { u8int p[] = {0x1E,0x20,0x20,0x1C,0x02,0x02,0x3C}; return p[row]; }
        case 'T': { u8int p[] = {0x3E,0x08,0x08,0x08,0x08,0x08,0x08}; return p[row]; }
        case 'U': { u8int p[] = {0x22,0x22,0x22,0x22,0x22,0x22,0x1C}; return p[row]; }
        case 'V': { u8int p[] = {0x22,0x22,0x22,0x22,0x22,0x14,0x08}; return p[row]; }
        case 'W': { u8int p[] = {0x22,0x22,0x22,0x2A,0x2A,0x2A,0x14}; return p[row]; }
        case 'X': { u8int p[] = {0x22,0x22,0x14,0x08,0x14,0x22,0x22}; return p[row]; }
        case 'Y': { u8int p[] = {0x22,0x22,0x14,0x08,0x08,0x08,0x08}; return p[row]; }
        case 'Z': { u8int p[] = {0x3E,0x02,0x04,0x08,0x10,0x20,0x3E}; return p[row]; }
        case '0': { u8int p[] = {0x1C,0x22,0x26,0x2A,0x32,0x22,0x1C}; return p[row]; }
        case '1': { u8int p[] = {0x08,0x18,0x08,0x08,0x08,0x08,0x1C}; return p[row]; }
        case '2': { u8int p[] = {0x1C,0x22,0x02,0x04,0x08,0x10,0x3E}; return p[row]; }
        case '3': { u8int p[] = {0x3C,0x02,0x02,0x1C,0x02,0x02,0x3C}; return p[row]; }
        case '4': { u8int p[] = {0x04,0x0C,0x14,0x24,0x3E,0x04,0x04}; return p[row]; }
        case '5': { u8int p[] = {0x3E,0x20,0x20,0x3C,0x02,0x02,0x3C}; return p[row]; }
        case '6': { u8int p[] = {0x1C,0x20,0x20,0x3C,0x22,0x22,0x1C}; return p[row]; }
        case '7': { u8int p[] = {0x3E,0x02,0x04,0x08,0x10,0x10,0x10}; return p[row]; }
        case '8': { u8int p[] = {0x1C,0x22,0x22,0x1C,0x22,0x22,0x1C}; return p[row]; }
        case '9': { u8int p[] = {0x1C,0x22,0x22,0x1E,0x02,0x02,0x1C}; return p[row]; }
        case '.': { u8int p[] = {0,0,0,0,0,0x08,0x08}; return p[row]; }
        case ':': { u8int p[] = {0,0x08,0x08,0,0x08,0x08,0}; return p[row]; }
        case '[': { u8int p[] = {0x1C,0x10,0x10,0x10,0x10,0x10,0x1C}; return p[row]; }
        case ']': { u8int p[] = {0x1C,0x04,0x04,0x04,0x04,0x04,0x1C}; return p[row]; }
        case '-': { u8int p[] = {0,0,0,0x1F,0,0,0}; return p[row]; }
        default:  { u8int p[] = {0,0,0,0,0,0,0}; return p[row]; }
    }
}
#endif

/* Write one character and its colour to a screen cell. */
void fb_write_cell(u32int index, char c, unsigned char fg, unsigned char bg) {
#if FB_MODE
    u32int cell_x = (index % SCREEN_WIDTH) * 8;
    u32int cell_y = (index / SCREEN_WIDTH) * 16;
    u32int fg_color = framebuffer_rgb(fg * 17, fg * 17, fg * 17);
    u32int bg_color = framebuffer_rgb(bg * 17, bg * 17, bg * 17);

    for (u32int row = 0; row < 16; row++) {
        for (u32int column = 0; column < 8; column++) {
            u32int x = cell_x + column;
            u32int y = cell_y + row;
            if (x < framebuffer.width && y < framebuffer.height)
                framebuffer_put_pixel(x, y, bg_color);
        }
    }

    if (c == '\0' || c == ' ')
        return;

    for (u32int row = 0; row < 16; row++) {
        u8int pattern = font_row(c, row / 2);
        for (u32int column = 0; column < 5; column++) {
            if ((pattern >> (4 - column)) & 1)
                framebuffer_put_pixel(cell_x + column, cell_y + row, fg_color);
        }
    }
#else
    u32int x = (index % SCREEN_WIDTH);
    u32int y = (index / SCREEN_WIDTH);
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
        return;
    unsigned char colour = FB_COLOUR(fg, bg);
    VGA_MEMORY[y * SCREEN_WIDTH + x] = ((unsigned short)colour << 8) | (unsigned char)c;
#endif
}

void fb_clear(unsigned char background) {
#if FB_MODE
    u32int color = framebuffer_rgb(background * 17, background * 17, background * 17);
    for (u32int y = 0; y < framebuffer.height; y++) {
        u32int *row = (u32int *)((u8int *)framebuffer.address + y * framebuffer.pitch);
        for (u32int x = 0; x < framebuffer.pitch / 4; x++)
            row[x] = color;
    }
#else
    attribute = (background << 4) | foreground;

    for (unsigned int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        VGA_MEMORY[i] = ((unsigned short)attribute << 8) | ' ';
    }
#endif
    fb_move_cursor(0);
}


void fb_move_cursor(unsigned short pos) {
#if !FB_MODE
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
#else
    (void)pos;
#endif
}

void fb_scroll(void) {
#if FB_MODE
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
#else
    (void)0;
#endif
}
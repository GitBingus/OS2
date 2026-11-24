#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "multiboot.h"
#include "types.h"
#include "paging.h"
#include "kstring.h"
#include "tui.h"
#include "pmm.h"

#define GRAPHICS_WIDTH  320
#define GRAPHICS_HEIGHT 200
#define GRAPHICS_BPP    8
#define GRAPHICS_MAX_COLOURS 256

#define GP_PAGE_SIZE PAGE_SIZE

void graphics_init(multiboot_info_t *mb_info);

typedef struct {
    u8int red;
    u8int green;
    u8int blue;
} multiboot_palette_entry_t;

typedef struct {
    unsigned int physical_address;  // where the fb stores the memory
    volatile u8int *pixels;         // virtual address kernel will use to write pixels
    unsigned int pitch;             // number of bytes occupied by one fb row
    unsigned int width;             // fb width
    unsigned int height;            // fb height
    unsigned char bpp;              // fb bits per pixel, for colour
    unsigned char type;             // fb colour organisation
    u8int red_position;             // which bits represent red, green and blue in each pixel 
    u8int red_mask_size;
    u8int green_position;
    u8int green_mask_size;
    u8int blue_position;
    u8int blue_mask_size;
} graphics_framebuffer_t;

void graphics_putpixel(
    u32int x,
    u32int y,
    u8int red,
    u8int green,
    u8int blue
);

void graphics_clear(u8int red, u8int green, u8int blue);

void graphics_draw_rect(
    u32int x,
    u32int y,
    u32int width,
    u32int height,
    u8int red,
    u8int green,
    u8int blue
);

void graphics_rgb_test(void);

#endif
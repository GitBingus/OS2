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
#define GRAPHICS_BPP    32
#define GRAPHICS_MAX_COLOURS 2**24
#define RGB(r, g, b) \
    (((u32int)(r) << 16) | ((u32int)(g) << 8) | (u32int)(b))

#define RGB_RED(colour)   (((colour) >> 16) & 0xFF)
#define RGB_GREEN(colour) (((colour) >> 8)  & 0xFF)
#define RGB_BLUE(colour)  ((colour) & 0xFF)

#define GP_PAGE_SIZE PAGE_SIZE
#define BG RGB(0, 0, 0) // black

bool graphics_init(multiboot_info_t *mb_info);

u32int graphics_get_width(void);
u32int graphics_get_height(void);
void graphics_scroll(u32int pixels, u8int red, u8int green, u8int blue);

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
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "multiboot.h"
#include "types.h"
#include "paging.h"

#define GRAPHICS_WIDTH  320
#define GRAPHICS_HEIGHT 200
#define GRAPHICS_BPP    8

#define GP_PAGE_SIZE PAGE_SIZE

void graphics_init(multiboot_info_t *mb_info);
void graphics_clear(unsigned int colour);

void graphics_putpixel(
    unsigned int x,
    unsigned int y,
    unsigned int colour
);

typedef struct {
    unsigned int physical_address;  // where the fb stores the memory
    volatile u8int *pixels;         // virtual address kernel will use to write pixels
    unsigned int pitch;             // number of bytes occupied by one fb row
    unsigned int width;             // fb width
    unsigned int height;            // fb height
    unsigned char bpp;              // fb bits per pixel, for colour
    unsigned char type;             // fb colour organisation
} graphics_framebuffer_t;

void graphics_draw_rect(
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height,
    unsigned int colour
);

#endif
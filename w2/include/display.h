#ifndef DISPLAY_H
#define DISPLAY_H


#include "kbool.h"
#include "types.h"
#include "kstring.h"
#include "font.h"
#include "graphics.h"
#include "fb.h"


typedef enum {
    DISPLAY_MODE_NONE,
    DISPLAY_MODE_TEXT,
    DISPLAY_MODE_GRAPHICS
} display_mode_t;

// Packs RGB channels into 0xRRGGBB.
#define RGB(r, g, b) \
    (((u32int)(r) << 16) | ((u32int)(g) << 8) | (u32int)(b))

bool display_init(display_mode_t mode);
bool display_set_mode(display_mode_t mode);
display_mode_t display_get_mode(void);
bool display_is_initialised(void);

// General display IO
void display_clear(void);
void display_write(cstring c);
void display_putchar(unsigned char c);
void display_write(cstring c);

u32int display_get_cursor(void);
void display_set_cursor(u32int position);
void display_newline(void);

// Text colours: VGA indices in text mode, RGB packed values in graphics mode.
void display_setcolour(u32int fg, u32int bg);

// Graphics-only operations
void display_putpixel(
    u32int x,
    u32int y,
    u8int red,
    u8int green,
    u8int blue
);

void display_draw_rect(
    u32int x,
    u32int y,
    u32int width,
    u32int height,
    u32int colour
);

#endif

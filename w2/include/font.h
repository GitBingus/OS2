#ifndef FONT_H
#define FONT_H

#include "types.h"

void font_init(void);

void font_draw_char(
    u32int x,
    u32int y,
    unsigned char c,
    u32int foreground,
    u32int background
);

#endif
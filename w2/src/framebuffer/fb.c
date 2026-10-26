#include "io.h"
#include "fb.h"
#include "tui.h"
#include "kbool.h"

/* Write one character and its colour to a screen cell. */
void fb_write_cell(unsigned int i, char c,
                   unsigned char fg, unsigned char bg)
{
    if (i >= SCREEN_WIDTH * SCREEN_HEIGHT)
        return;

    unsigned char attribute = FB_COLOUR(fg, bg);

    VGA_MEMORY[i] = ((unsigned short)attribute << 8) |
                    (unsigned char)c;
}

void fb_clear(unsigned char background)
{
    const unsigned char foreground = 0x0F;
    const unsigned char attribute = (background << 4) | foreground;

    for (unsigned int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        VGA_MEMORY[i] = ((unsigned short)attribute << 8) | ' ';
    }

    fb_move_cursor(0);
}


void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}
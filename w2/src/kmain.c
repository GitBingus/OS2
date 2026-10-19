#include "io.h"

#define VGA_MEMORY ((volatile unsigned short *)0xB8000)

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

/* VGA colour values */
#define FB_BLACK       0
#define FB_GREEN       2
#define FB_DARK_GREY   8
#define FB_WHITE       15

/*
 * The VGA attribute byte uses:
 * bits 0-3: foreground colour
 * bits 4-6: background colour
 */

#define FB_COLOUR(fg, bg) (((bg) << 4) | (fg))

int sum_of_three(int arg1, int arg2, int arg3)
{
    return arg1 + arg2 + arg3;
}

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

/* Fill the entire screen with a dark green background. */
void fb_clear(unsigned char bg)
{
    for (unsigned int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        fb_write_cell(i, ' ', FB_WHITE, bg);
    }
}

void kmain(void)
{
    // Fill the screen with green background and black text.
    for (unsigned int i = 0; i < 80 * 25; i++) {
        VGA_MEMORY[i] = (unsigned short)(0x20 << 8) | ' ';
    }

    // Put a white A in the top-left corner.
    VGA_MEMORY[0] = (unsigned short)(0x2F << 8) | 'N';
    VGA_MEMORY[1] = (unsigned short)(0x2F << 8) | 'I';
    VGA_MEMORY[2] = (unsigned short)(0x2F << 8) | 'G';
    VGA_MEMORY[3] = (unsigned short)(0x2F << 8) | 'G';
    VGA_MEMORY[4] = (unsigned short)(0x2F << 8) | 'A';

    fb_move_cursor(0);
}

void print_result(int value)
{
    const char *label = "Result: ";
    int position = 0;

    while (*label != '\0') {
        VGA_MEMORY[position++] =
            (FB_COLOUR(FB_WHITE, FB_BLACK) << 8) |
            (unsigned char)*label++;
    }

    if (value < 0) {
        VGA_MEMORY[position++] =
            (FB_COLOUR(FB_WHITE, FB_BLACK) << 8) | '-';
        value = -value;
    }

    char digits[12];
    int count = 0;

    do {
        digits[count++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    while (count > 0) {
        VGA_MEMORY[position++] =
            (FB_COLOUR(FB_WHITE, FB_BLACK) << 8) |
            (unsigned char)digits[--count];
    }
}

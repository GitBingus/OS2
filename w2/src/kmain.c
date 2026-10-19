#include "io.h"
#include <stdbool.h>

#define VGA_MEMORY ((volatile unsigned short *)0xB8000)

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

/* VGA colour values */
#define FB_BLACK       0
#define FB_GREEN       2
#define FB_DARK_GREY   8
#define FB_WHITE       15

#define KBD_STATUS_PORT         0x64
#define KBD_DATA_PORT           0x60

/*
 * The VGA attribute byte uses:
 * bits 0-3: foreground colour
 * bits 4-6: background colour
 */

// TODO: FIX KEYBOARD INPUT

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

    unsigned short cursor = 0;

    bool extended = false; // Used for extended keys, like arrow keys

    while (1)
    {
        // Check whether keyboard data is available.
        unsigned char status;

        __asm__ volatile (
            "inb %1, %0"
            : "=a"(status)
            : "Nd"(KBD_STATUS_PORT)
        );

        if (!(status & 0x01))
            continue;


        // Read the scan code.
        unsigned char scan_code = read_scan_code();

        if (extended) {
            // Handle extended keys here if needed.
            // Up: 48 Down: 50 Left: 4B Right: 4D

            if (scan_code == 0x48) { // Up arrow
                if (cursor >= SCREEN_WIDTH)
                    cursor -= SCREEN_WIDTH;
            } else if (scan_code == 0x50) { // Down arrow
                if (cursor < SCREEN_WIDTH * (SCREEN_HEIGHT - 1))
                    cursor += SCREEN_WIDTH;
            } else if (scan_code == 0x4B) { // Left arrow
                if (cursor > 0)
                    cursor--;
            } else if (scan_code == 0x4D) { // Right arrow
                if (cursor < SCREEN_WIDTH * SCREEN_HEIGHT - 1)
                    cursor++;
            }

            extended = false;
            continue;
        }

        if (scan_code == 0xE0) {
            extended = true;
            
            continue;
        }

        // Ignore key-release events for now.
        if (scan_code & 0x80)
            continue;

        // Convert to ASCII.
        char character = scan_code_to_ascii(scan_code);

        if (character != 0)
        {
            // Write the character to VGA memory.
            VGA_MEMORY[cursor] = (0x0F << 8) | character;

            cursor++;

            // Wrap around when reaching the end.
            if (cursor >= 80 * 25)
                cursor = 0;

            // Update hardware cursor.
            fb_move_cursor(cursor);
        }
    }
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

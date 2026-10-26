#include "io.h"
#include "fb.h"
#include "tui.h"
#include "kbool.h"
#include "gdt.h"

void kmain(void)
{
    fb_clear(COLOUR.BLACK); // Clears the screen

    setGDT();   // inits GDT

    unsigned char foreground = COLOUR.WHITE;
    unsigned char background = COLOUR.BLACK;

    unsigned char attribute = (background << 4) | foreground;

    unsigned short cursor = 0;
    bool extended = false;
    bool shift_pressed = false;
    bool caps_lock = false;

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

            fb_move_cursor(cursor);

            extended = false;
            continue;
        }

        // Shift pressed
        if (scan_code == 0x2A || scan_code == 0x36)
        {
            shift_pressed = true;
            continue;
        }

        // Shift released
        if (scan_code == 0xAA || scan_code == 0xB6)
        {
            shift_pressed = false;
            continue;
        }

        // Caps Lock pressed
        if (scan_code == 0x3A)
        {
            caps_lock = !caps_lock;
            continue;
        }

        if (scan_code == 0xE0) {
            extended = true;
            
            continue;
        }

        // Ignore key-release events for now.
        // if (scan_code & 0x80)
        //      continue;

        // ENTER
        if (scan_code == 0x1C)
        {
            cursor = ((cursor / SCREEN_WIDTH) + 1) * SCREEN_WIDTH;

            if (cursor >= SCREEN_WIDTH * SCREEN_HEIGHT)
                cursor = 0;

            fb_move_cursor(cursor);
            continue;
        }

        // BACKSPACE
        if (scan_code == 0x0E)
        {
            if (cursor > 0)
            {
                cursor--;

                // Remove the previous character by replacing it with a space.
                VGA_MEMORY[cursor] =
                    ((unsigned short)attribute << 8) | ' ';

                fb_move_cursor(cursor);
            }

            continue;
        }

        // TAB
        if (scan_code == 0x0F)
        {
            // Move to the next multiple of 4 columns.
            unsigned short column = cursor % SCREEN_WIDTH;
            unsigned short spaces = 4 - (column % 4);

            cursor += spaces;

            if (cursor >= SCREEN_WIDTH * SCREEN_HEIGHT)
                cursor = 0;

            fb_move_cursor(cursor);
            continue;
        }

        char character = scan_code_to_ascii(scan_code); // Convert scan code to ASCII character

        if (character >= 'a' && character <= 'z')   // Lowercase letters 
        {
            if (shift_pressed != caps_lock)         // Convert to uppercase if shift is pressed or caps lock is on (but not both)
                character -= ('a' - 'A');
        }

        if (character != 0) // Only write valid characters
        {
            // Write the character to VGA memory.
            VGA_MEMORY[cursor] = ((unsigned short)attribute << 8) | (unsigned char)character;

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

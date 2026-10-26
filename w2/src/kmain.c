#include "io.h"
#include "fb.h"
#include "tui.h"
#include "kbool.h"
#include "gdt.h"
#include "keyboard.h"


void kmain(void)
{
    // Initialise the framebuffer and GDT.
    fb_clear(COLOUR.BLACK);
    setGDT();

    const unsigned char foreground = COLOUR.WHITE;
    const unsigned char background = COLOUR.BLACK;
    const unsigned char attribute = FB_COLOUR(foreground, background);

    const unsigned short screen_size = SCREEN_WIDTH * SCREEN_HEIGHT;

    unsigned short cursor = 0;

    // VGA text memory is memory-mapped hardware.
    volatile unsigned short *vga = VGA_MEMORY;

    fb_move_cursor(cursor);

    while (1)
    {
        // Read and interpret any available keyboard input.
        keyboard_event_t event = keyboard_poll();

        if (event.key == KEY_NONE)
            continue;

        switch (event.key)
        {
            // Move cursor upwards.
            case KEY_UP:
                if (cursor >= SCREEN_WIDTH)
                    cursor -= SCREEN_WIDTH;
                break;

            // Move cursor downwards.
            case KEY_DOWN:
                if (cursor < SCREEN_WIDTH * (SCREEN_HEIGHT - 1))
                    cursor += SCREEN_WIDTH;
                break;

            // Move cursor left.
            case KEY_LEFT:
                if (cursor > 0)
                    cursor--;
                break;

            // Move cursor right.
            case KEY_RIGHT:
                if (cursor < screen_size - 1)
                    cursor++;
                break;

            // Move to the beginning of the next line.
            case KEY_ENTER:
                cursor = ((cursor / SCREEN_WIDTH) + 1) * SCREEN_WIDTH;

                if (cursor >= screen_size)
                    cursor = 0;
                break;

            // Remove the character before the cursor.
            case KEY_BACKSPACE:
                if (cursor > 0)
                {
                    cursor--;

                    vga[cursor] =
                        ((unsigned short)attribute << 8) | ' ';
                }
                break;

            // Advance to the next tab stop (every 4 columns).
            case KEY_TAB:
            {
                unsigned short column = cursor % SCREEN_WIDTH;
                unsigned short spaces = 4 - (column % 4);

                cursor += spaces;

                if (cursor >= screen_size)
                    cursor = 0;

                break;
            }

            // Display a regular character.
            case KEY_CHARACTER:
                vga[cursor] =
                    ((unsigned short)attribute << 8)
                    | (unsigned char)event.character;

                cursor++;

                if (cursor >= screen_size)
                    cursor = 0;

                break;

            case KEY_NONE:
            default:
                break;
        }

        // Update the hardware cursor after processing the event.
        fb_move_cursor(cursor);
    }
}
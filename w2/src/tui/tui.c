#include "fb.h"
#include "tui.h"

static unsigned short tui_cursor = 0;

void tui_init(void) {   // init tui
    tui_cursor = 0;
    fb_clear(FB_BLACK);

    fb_move_cursor(0); 
}

void tui_putchar(char c) {
    /* write one character at the current cursor position, advance the cursor and update the hardware cursor */

    if (c == '\n') {
        tui_newline();
        return;
    }

    fb_write_cell(tui_cursor, c, foreground, background);
    tui_cursor++;

    if (tui_cursor >= screen_size) tui_scroll();

    fb_move_cursor(tui_cursor);

}

void tui_write(cstring str) {
    while (*str != '\0')
    {
        tui_putchar(*str);
        str++;
    }
}

void tui_handle_key(keyboard_event_t event) {
    switch (event.key)
    {
        case KEY_CHARACTER:
            tui_putchar(event.character);

            break;

        case KEY_ENTER:
            tui_newline();

            break;

        case KEY_BACKSPACE:
            tui_backspace();

            break;

        case KEY_TAB:
            tui_tab();

            break;

        case KEY_UP:
            tui_move_up();

            break;

        case KEY_DOWN:
            tui_move_down();

            break;

        case KEY_LEFT:
            tui_move_left();

            break;

        case KEY_RIGHT:
            tui_move_right();

            break;

        case KEY_NONE:
        default:
            break;
    }
}

void tui_tab(void)
{
    // Find the current column on the screen
    unsigned short column = tui_cursor % SCREEN_WIDTH;

    // Calculate spaces until the next tab stop
    unsigned short spaces = 4 - (column % 4);

    // Move the TUI cursor
    tui_cursor += spaces;

    // Wrap around if we reach the end of the screen
    if (tui_cursor >= SCREEN_WIDTH * SCREEN_HEIGHT)
        tui_cursor = 0;

    // Update the VGA hardware cursor
    fb_move_cursor(tui_cursor);
}

void tui_move_up(void) {
    if (tui_cursor >= SCREEN_WIDTH) tui_cursor -= SCREEN_WIDTH;

    fb_move_cursor(tui_cursor);
}

void tui_move_down(void) {
    if (tui_cursor < SCREEN_WIDTH * (SCREEN_HEIGHT - 1)) tui_cursor += SCREEN_WIDTH;

    fb_move_cursor(tui_cursor);
}

void tui_move_left(void)
{
    if (tui_cursor > 0) tui_cursor--;

    fb_move_cursor(tui_cursor);
}

void tui_move_right(void) {
    if (tui_cursor < (unsigned int)screen_size - 1) tui_cursor++;

    fb_move_cursor(tui_cursor);
}

void tui_newline(void) {
    tui_cursor = ((tui_cursor / SCREEN_WIDTH) + 1) * SCREEN_WIDTH;
    if (tui_cursor >= screen_size) tui_scroll();

    fb_move_cursor(tui_cursor);
}

void tui_backspace(void)
{
    if (tui_cursor > 0) {
        tui_cursor--;

        fb_write_cell(tui_cursor, ' ', foreground, background);
    }

    fb_move_cursor(tui_cursor);
}

void tui_scroll(void)
{
    // move rows 1–24 up into rows 0–23
    for (unsigned int row = 0; row < SCREEN_HEIGHT - 1; row++)
    {
        for (unsigned int col = 0; col < SCREEN_WIDTH; col++)
        {
            unsigned int destination = row * SCREEN_WIDTH + col;
            unsigned int source = (row + 1) * SCREEN_WIDTH + col;

            VGA_MEMORY[destination] = VGA_MEMORY[source];
        }
    }

    // clear the final row
    unsigned short attribute = ((unsigned short)background << 4) | foreground;

    for (unsigned int col = 0; col < SCREEN_WIDTH; col++)
    {
        unsigned int position = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH + col;

        VGA_MEMORY[position] = (attribute << 8) | ' ';
    }

    // position cursor at the beginning of the last row
    tui_cursor = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH;

    fb_move_cursor(tui_cursor);
}
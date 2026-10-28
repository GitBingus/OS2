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

    fb_write_cell(tui_cursor, c, foreground, background);
    tui_cursor++;

    if (tui_cursor >= screen_size)
        tui_cursor = 0;

    fb_move_cursor(tui_cursor);

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
    if (tui_cursor >= screen_size) tui_cursor = 0;

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
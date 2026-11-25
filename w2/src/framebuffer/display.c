#include "display.h"

static display_mode_t current_mode = DISPLAY_MODE_NONE;
static bool initialised = false;

static u32int foreground_colour = RGB(255, 255, 255);
static u32int background_colour = RGB(0, 0, 0);

static u32int gfx_cursor_x = 0;
static u32int gfx_cursor_y = 0;

static void gfx_cursor_reset(void) {
    gfx_cursor_x = 0;
    gfx_cursor_y = 0;
}

// Initialise the display manager and select the initial display backend.
bool display_init(display_mode_t mode) {
    if (initialised) return false;

    if (!display_set_mode(mode)) return false;

    initialised = true;
    return true;
}

// Select the active display backend.
bool display_set_mode(display_mode_t mode) {
    if (mode != DISPLAY_MODE_GRAPHICS &&
        mode != DISPLAY_MODE_TEXT)
        return false;

    current_mode = mode;
    return true;
}

// Return the current display mode.
display_mode_t display_get_mode(void) {
    return current_mode;
}

// Check whether the display manager has initialised.
bool display_is_initialised(void) {
    return initialised;
}

void display_clear(void) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS:
            graphics_clear(
                RGB_RED(BG),
                RGB_GREEN(BG),
                RGB_BLUE(BG)
            );

            gfx_cursor_reset(); 
            break;

        case DISPLAY_MODE_TEXT:
            tui_clear();
            break;

        default:
            break;
    }
}

void display_setcolour(u32int fg, u32int bg) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_TEXT:

            // VGA: 4-bit colour indices
            if (fg > 0x0F || bg > 0x0F)
                return;

            foreground = fg;
            background = bg;

            break;

        case DISPLAY_MODE_GRAPHICS:

            // RGB: 24-bit colour values
            if (fg > 0xFFFFFF || bg > 0xFFFFFF)
                return;

            foreground_colour = fg;
            background_colour = bg;

            break;

        default:
            return;
    }
}

static void gfx_newline(void) {
    gfx_cursor_x = 0;
    gfx_cursor_y += font_get_line_height();
    /* TODO: scroll when gfx_cursor_y + line height > screen height */
}

void display_putchar(unsigned char c) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS:
            switch (c) {
                case '\n':
                    gfx_newline();
                    break;

                case '\r':
                    gfx_cursor_x = 0;
                    break;

                case '\t': {
                    u32int tab_width = font_get_advance(' ') * 4;
                    gfx_cursor_x = ((gfx_cursor_x / tab_width) + 1) * tab_width;
                    break;
                }

                default:
                    if (c < 32 || c > 126) {
                        break;  // ignore unprintable characters
                    }

                    if (gfx_cursor_x + font_get_advance(c) > GRAPHICS_WIDTH) {
                        gfx_newline();
                    }

                    font_draw_char(gfx_cursor_x, gfx_cursor_y, c,
                                   foreground_colour, background_colour);

                    gfx_cursor_x += font_get_advance(c);
                    break;
            }
            break;

        case DISPLAY_MODE_TEXT:
            tui_putchar(c);
            break;

        default:
            break;
    }
}
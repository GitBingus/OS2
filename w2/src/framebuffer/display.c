#include "display.h"

static display_mode_t current_mode = DISPLAY_MODE_NONE;
static bool initialised = false;

static u32int foreground_colour;
static u32int background_colour;

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

void display_putchar(unsigned char c) {
    (void)c;
}
#include "display.h"
#include "tui.h"

static display_mode_t current_mode = DISPLAY_MODE_NONE;
static bool initialised = false;

static u32int foreground_colour = RGB(255, 255, 255);
static u32int background_colour = RGB(0, 0, 0);

static u32int gfx_cursor_x = 0;
static u32int gfx_cursor_y = 0;
static bool gfx_cursor_visible = false;

static void gfx_cursor_draw(bool show);

static void gfx_cursor_reset(void) {
    gfx_cursor_x = 0;
    gfx_cursor_y = 0;
    gfx_cursor_visible = false;
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

void display_draw_rect(u32int x, u32int y, u32int w, u32int h, u32int colour) {
    if (display_get_mode() != DISPLAY_MODE_GRAPHICS) return;   /* no meaning in VGA text mode */

    for (u32int row = 0; row < h; row++) {
        for (u32int col = 0; col < w; col++) {
            u32int px = x + col;
            u32int py = y + row;

            /* clip to the screen so the cursor can't write past the framebuffer */
            if (px >= GRAPHICS_WIDTH || py >= GRAPHICS_HEIGHT) continue;

            graphics_putpixel(px, py, RGB_RED(colour), RGB_GREEN(colour), RGB_BLUE(colour));
        }
    }
}

void display_clear(void) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS:
            graphics_clear(
                RGB_RED(background_colour),
                RGB_GREEN(background_colour),
                RGB_BLUE(background_colour)
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

static u32int display_columns(void) {
    if (display_get_mode() == DISPLAY_MODE_GRAPHICS) {
        u32int advance = font_get_advance(' ');
        return advance ? GRAPHICS_WIDTH / advance : 1;
    }
    return SCREEN_WIDTH;
}

u32int display_get_cursor(void) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS: {
            u32int row = gfx_cursor_y / font_get_line_height();
            u32int col = gfx_cursor_x / font_get_advance(' ');
            return row * display_columns() + col;
        }
        case DISPLAY_MODE_TEXT:
            return tui_get_cursor();
        default:
            return 0;
    }
}

void display_set_cursor(u32int position) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS: {
            if (gfx_cursor_visible) gfx_cursor_draw(false);   /* erase at old position */

            u32int columns = display_columns();
            gfx_cursor_x = (position % columns) * font_get_advance(' ');
            gfx_cursor_y = (position / columns) * font_get_line_height();

            gfx_cursor_draw(true);
            break;
        }
        case DISPLAY_MODE_TEXT:
            tui_set_cursor(position);
            break;
        default:
            break;
    }
}

static void gfx_newline(void) {
    if (gfx_cursor_visible) gfx_cursor_draw(false);   // erase before moving 
    gfx_cursor_x = 0;
    gfx_cursor_y += font_get_line_height();
}
void display_putchar(unsigned char c) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS:
            if (gfx_cursor_visible) gfx_cursor_draw(false);
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

static void gfx_cursor_draw(bool show) {
    u32int w = font_get_advance(' ');
    u32int h = font_get_line_height();
    u32int colour = show ? foreground_colour : background_colour;

    /* 2-pixel underline along the bottom of the cell */
    display_draw_rect(gfx_cursor_x, gfx_cursor_y + h - 2, w, 2, colour);
    gfx_cursor_visible = show;
}

static void display_cursor_update(void) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_TEXT:
            tui_update_cursor();
            break;

        case DISPLAY_MODE_GRAPHICS:
            gfx_cursor_draw(true);
            break;

        default:
            break;
    }
}

/* Helpers: the only places that care which backend is active */

static bool display_at_line_start(void) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS: return gfx_cursor_x == 0;
        case DISPLAY_MODE_TEXT:     return tui_get_column() == 0;
        default:                    return true;
    }
}

static u32int display_line_space(void) {
    switch (display_get_mode()) {
        case DISPLAY_MODE_GRAPHICS:
            return (gfx_cursor_x >= GRAPHICS_WIDTH) ? 0 : GRAPHICS_WIDTH - gfx_cursor_x;
        case DISPLAY_MODE_TEXT:
            return SCREEN_WIDTH - tui_get_column();
        default:
            return 0;
    }
}

/* Width of the next word in the same unit as display_line_space() */
static u32int display_word_width(cstring s, u32int length) {
    if (display_get_mode() == DISPLAY_MODE_GRAPHICS) {
        u32int width = 0;
        for (u32int i = 0; i < length; i++) {
            width += font_get_advance((unsigned char)s[i]);
        }
        return width;
    }
    return length;   /* one cell per character in text mode */
}

void display_newline(void) {
    if (display_get_mode() == DISPLAY_MODE_GRAPHICS) {
        gfx_newline();
    } else {
        tui_newline();
    }
}

void display_write(cstring c) {
    if (!c) return;

    display_mode_t mode = display_get_mode();
    if (mode != DISPLAY_MODE_GRAPHICS && mode != DISPLAY_MODE_TEXT) return;

    /* Erase the software cursor before the text moves it */
    if (mode == DISPLAY_MODE_GRAPHICS && gfx_cursor_visible) {
        gfx_cursor_draw(false);
    }

    while (*c != '\0') {
        if (*c == '\n') {
            display_newline();
            c++;
            continue;
        }

        if (*c == ' ') {
            display_putchar((unsigned char)*c++);
            continue;
        }

        /* Length of the next word, in characters */
        u32int length = 0;
        while (c[length] != '\0' && c[length] != ' ' && c[length] != '\n') {
            length++;
        }

        /* Wrap before the word if it won't fit on this line */
        if (!display_at_line_start() &&
            display_word_width(c, length) > display_line_space()) {
            display_newline();
        }

        for (u32int i = 0; i < length; i++) {
            display_putchar((unsigned char)*c++);
        }
    }

    display_cursor_update();
}
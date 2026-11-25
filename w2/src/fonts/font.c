#include "font.h"
#include "consolas_12.inl"

#include "display.h"

static stb_fontchar fontdata[
    STB_FONT_consolas_12_usascii_NUM_CHARS
];

static unsigned char fontpixels[
        STB_FONT_consolas_12_usascii_BITMAP_HEIGHT
    ][STB_FONT_consolas_12_usascii_BITMAP_WIDTH
];

void font_init(void) {
    stb_font_consolas_12_usascii(
        fontdata,
        fontpixels,
        STB_FONT_consolas_12_usascii_BITMAP_HEIGHT
    );
}

void font_draw_char(
    u32int x,
    u32int y,
    unsigned char c,
    u32int foreground,
    u32int background
) {
    if (!((c >= 32) && (c <= 126))) return; // out of bounds

    // Calculate glyph index
    u32int index = c - STB_FONT_consolas_12_usascii_FIRST_CHAR;

    // Retrieve glyph metadata
    stb_fontchar *glyph = &fontdata[index];

    int width = glyph->x1 - glyph->x0;
    int height = glyph->y1 - glyph->y0;
    int atlas_x = (int)(
        glyph->s0 * STB_FONT_consolas_12_usascii_BITMAP_WIDTH
    );

    int atlas_y = (int)(
        glyph->t0 * STB_FONT_consolas_12_usascii_BITMAP_HEIGHT
    );

    // Iterate through glyph pixels 

    for (int gy = 0; gy < height; gy++) {
        for (int gx = 0; gx < width; gx++) {

            unsigned char pixel =
                fontpixels[atlas_y + gy][atlas_x + gx];

            // Screen position:
            int screen_x = (int)x + glyph->x0 + gx;
            int screen_y = (int)y + glyph->y0 + gy;

            if (pixel > 0) {
                // Extract RGB from foreground and draw pixel

                u8int red   = RGB_RED(foreground);
                u8int green = RGB_GREEN(foreground);
                u8int blue  = RGB_BLUE(foreground);

                graphics_putpixel(screen_x, screen_y, red, green, blue);
            } else {
                // Extract RGB from background and draw pixel

                u8int red   = RGB_RED(background);
                u8int green = RGB_GREEN(background);
                u8int blue  = RGB_BLUE(background);

                graphics_putpixel(screen_x, screen_y, red, green, blue);
            }

        }
    }
}

u32int font_get_advance(unsigned char c) {
    if (!((c >= 32) && (c <= 126))) return 0; // out of bounds

    // Calculate glyph index
    u32int index = c - STB_FONT_consolas_12_usascii_FIRST_CHAR;

    // Retrieve glyph metadata
    stb_fontchar *glyph = &fontdata[index];

    return (u32int)glyph->advance_int;
}

u32int font_get_line_height(void) {
    return (u32int)STB_FONT_consolas_12_usascii_LINE_SPACING;
}
#include "font.h"
#include "consolas.inl"

#include "display.h"

static stb_fontchar fontdata[
    STB_FONT_consolas_22_usascii_NUM_CHARS
];

static unsigned char fontpixels[
        STB_FONT_consolas_22_usascii_BITMAP_HEIGHT
    ][STB_FONT_consolas_22_usascii_BITMAP_WIDTH
];

void font_init(void) {
    stb_font_consolas_22_usascii(
        fontdata,
        fontpixels,
        STB_FONT_consolas_22_usascii_BITMAP_HEIGHT
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
    u32int index = c - STB_FONT_consolas_22_usascii_FIRST_CHAR;

    // Retrieve glyph metadata
    stb_fontchar *glyph = &fontdata[index];

    int width = glyph->x1 - glyph->x0;
    int height = glyph->y1 - glyph->y0;
    int atlas_x = (int)(
        glyph->s0 * STB_FONT_consolas_22_usascii_BITMAP_WIDTH
    );

    int atlas_y = (int)(
        glyph->t0 * STB_FONT_consolas_22_usascii_BITMAP_HEIGHT
    );

    
    graphics_draw_rect(x, y, font_get_advance(c), font_get_line_height(),
                       RGB_RED(background), RGB_GREEN(background), RGB_BLUE(background));

    // Iterate through glyph pixels 

    for (int gy = 0; gy < height; gy++) {
        for (int gx = 0; gx < width; gx++) {
            u8int coverage = fontpixels[atlas_y + gy][atlas_x + gx];

            // Screen position
            int screen_x = (int)x + glyph->x0 + gx;
            int screen_y = (int)y + glyph->y0 + gy;

            if (screen_x < 0 || screen_y < 0) continue;

            u32int colour = font_blend(foreground, background, coverage);

            graphics_putpixel((u32int)screen_x, (u32int)screen_y, RGB_RED(colour),
            RGB_GREEN(colour),
            RGB_BLUE(colour));
        }
    }
}

u32int font_blend(u32int fg, u32int bg, u8int coverage) {
    u32int r = (RGB_RED(fg)   * coverage + RGB_RED(bg)   * (255 - coverage)) / 255;
    u32int g = (RGB_GREEN(fg) * coverage + RGB_GREEN(bg) * (255 - coverage)) / 255;
    u32int b = (RGB_BLUE(fg)  * coverage + RGB_BLUE(bg)  * (255 - coverage)) / 255;
    return RGB(r, g, b);
}

u32int font_get_advance(unsigned char c) {
    if (!((c >= 32) && (c <= 126))) return 0; // out of bounds

    // Calculate glyph index
    u32int index = c - STB_FONT_consolas_22_usascii_FIRST_CHAR;

    // Retrieve glyph metadata
    stb_fontchar *glyph = &fontdata[index];

    return (u32int)glyph->advance_int;
}

u32int font_get_line_height(void) {
    return 16;  
}
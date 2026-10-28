#ifndef FB_H
#define FB_H

#define FB_COLOUR(fg, bg) (((bg) << 4) | (fg))

#define VGA_MEMORY ((volatile unsigned short *)0xB8000)

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/* VGA colour values */
#define FB_BLACK       0
#define FB_GREEN       2
#define FB_DARK_GREY   8
#define FB_WHITE       15

typedef struct {
    unsigned char BLACK;
    unsigned char BLUE;
    unsigned char GREEN;
    unsigned char CYAN;
    unsigned char RED;
    unsigned char MAGENTA;
    unsigned char BROWN;
    unsigned char LIGHT_GREY;
    unsigned char DARK_GREY;
    unsigned char LIGHT_BLUE;
    unsigned char LIGHT_GREEN;
    unsigned char LIGHT_CYAN;
    unsigned char LIGHT_RED;
    unsigned char LIGHT_MAGENTA;
    unsigned char YELLOW;
    unsigned char WHITE;
} VGAColours;

static const VGAColours COLOUR = {
    .BLACK         = 0x0,
    .BLUE          = 0x1,
    .GREEN         = 0x2,
    .CYAN          = 0x3,
    .RED           = 0x4,
    .MAGENTA       = 0x5,
    .BROWN         = 0x6,
    .LIGHT_GREY    = 0x7,
    .DARK_GREY     = 0x8,
    .LIGHT_BLUE    = 0x9,
    .LIGHT_GREEN   = 0xA,
    .LIGHT_CYAN    = 0xB,
    .LIGHT_RED     = 0xC,
    .LIGHT_MAGENTA = 0xD,
    .YELLOW        = 0xE,
    .WHITE         = 0xF
};

extern const unsigned char foreground;
extern const unsigned char background;
extern unsigned char attribute;
extern const unsigned short screen_size;

void fb_move_cursor(unsigned short pos);
void fb_clear(unsigned char background);
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);

#endif
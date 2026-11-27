#ifndef TETRIS_H
#define TETRIS_H

/*
Milestone	Objective
1	        Board representation and tetromino definitions
2	        Piece movement and collision detection
3	        Rotation
4	        Gravity and piece locking
5	        Line clearing and scoring
6	        Text-mode rendering
7	        Keyboard integration
8	        Game loop and pause functionality
9	        PC speaker audio
10	        Levels, next-piece preview and polish
11	        Graphical rendering
12	        More sophisticated audio
*/

#include "keyboard.h"
#include "types.h"
#include "tui.h"

#include "tetrominoes.h"

#define WIDTH               10
#define HEIGHT              20

#define MOVEMENT_LEFT       KEY_LEFT        // move left
#define MOVEMENT_RIGHT      KEY_RIGHT       // move right
#define MOVEMENT_DOWN       KEY_DOWN        // soft drop -- change gravity
#define MOVEMENT_UP         KEY_UP          // rotation
#define HARD_DROP           KEY_ENTER       // hard drop -- place piece immediately
#define EXIT                KEY_BACKSPACE   // exit game

#define EMPTY_CELL 0xFF

#define BOARD_SCREEN_WIDTH  (WIDTH * 2)
#define BOARD_SCREEN_HEIGHT HEIGHT

#define BOARD_START_X 10
#define BOARD_START_Y 2

static const unsigned char TETRIS_MOSAIC[20][10] = {
    { 0x0E, 0x0E, 0x01, 0x01, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06 },
    { 0x0E, 0x0E, 0x01, 0x05, 0x05, 0x02, 0x05, 0x0E, 0x0E, 0x06 },
    { 0x0E, 0x0E, 0x01, 0x05, 0x05, 0x02, 0x02, 0x0E, 0x0E, 0x06 },
    { 0x0E, 0x0E, 0x05, 0x05, 0x05, 0x04, 0x02, 0x05, 0x01, 0x01 },
    { 0x02, 0x01, 0x01, 0x01, 0x04, 0x04, 0x05, 0x05, 0x01, 0x03 },
    { 0x02, 0x02, 0x05, 0x01, 0x04, 0x02, 0x02, 0x05, 0x01, 0x03 },
    { 0x03, 0x02, 0x05, 0x05, 0x02, 0x02, 0x01, 0x01, 0x01, 0x03 },
    { 0x03, 0x05, 0x05, 0x04, 0x04, 0x01, 0x01, 0x01, 0x01, 0x03 },
    { 0x03, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x01, 0x02, 0x02 },
    { 0x03, 0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x02, 0x02, 0x04 },
    { 0x0E, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x0E, 0x0E, 0x04, 0x04 },
    { 0x0E, 0x0E, 0x01, 0x04, 0x02, 0x02, 0x0E, 0x0E, 0x04, 0x03 },
    { 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03 },
    { 0x01, 0x01, 0x01, 0x06, 0x03, 0x02, 0x06, 0x06, 0x03, 0x03 },
    { 0x01, 0x06, 0x06, 0x06, 0x03, 0x02, 0x02, 0x06, 0x03, 0x03 },
    { 0x0E, 0x0E, 0x0E, 0x0E, 0x03, 0x06, 0x02, 0x06, 0x03, 0x03 },
    { 0x0E, 0x0E, 0x0E, 0x0E, 0x03, 0x06, 0x06, 0x06, 0x03, 0x03 },
    { 0x06, 0x02, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x03 },
    { 0x06, 0x02, 0x02, 0x05, 0x05, 0x04, 0x04, 0x06, 0x06, 0x03 },
    { 0x06, 0x06, 0x02, 0x05, 0x05, 0x05, 0x04, 0x04, 0x06, 0x06 },
};

typedef struct {
    unsigned char cells[HEIGHT][WIDTH];
} tetris_board_t;

typedef struct {
    unsigned char tiles[6][9][9];
} tetris_splashscreen_t;

typedef struct {
    tetromino_type_t type;
    int x;
    int y;
    u8int tiles[4][4];
} tetris_piece_t;

void tetris_init(void);
void tetris_board_init(tetris_board_t *board);
bool tetris_board_full(const tetris_board_t *board, unsigned int y);
void tetris_board_clear(tetris_board_t *board, unsigned int y);
u32int tetris_board_clear_lines(tetris_board_t *board);
void tetris_board_render_border();

void tetris_piece_render(tetris_piece_t *piece);
void tetris_board_render(const tetris_board_t *board);

void tetris_board_load_mosaic(tetris_board_t *board);

static const tetris_splashscreen_t tetris_splashscreen = {
    .tiles  = {
        {   // T
            {0, 0, 1, 1, 1, 1, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
        }, { // E
            {1, 1, 1, 1, 1, 1, 1, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0},
        }, { // T
            {0, 0, 1, 1, 1, 1, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
        }, { // R
            {1, 1, 1, 1, 1, 0, 0, 0, 0},
            {1, 0, 0, 0, 1, 0, 0, 0, 0},
            {1, 0, 0, 0, 1, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 0, 0, 0, 0},
            {1, 0, 1, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 1, 0, 0, 0, 0, 0},
            {1, 0, 0, 1, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 1, 0, 0, 0, 0},
            {1 ,0, 0, 0, 1, 0, 0, 0, 0}
        }, { // I 
            {0, 1, 1, 1, 1, 1, 1, 1, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 1, 1, 1, 1, 1, 1, 1, 0},
        }, {// S
            {0, 1, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 1, 1, 1, 1, 1, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0},
            {0, 1, 1, 1, 1, 1, 1, 1, 0},
        }
    }
};

typedef struct { char c; u8int rows[7]; } splash_glyph_t;

static const splash_glyph_t splash_glyphs[] = {
    { 'P', {0b11110,0b10001,0b10001,0b11110,0b10000,0b10000,0b10000} },
    { 'R', {0b11110,0b10001,0b10001,0b11110,0b10100,0b10010,0b10001} },
    { 'E', {0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b11111} },
    { 'S', {0b01111,0b10000,0b10000,0b01110,0b00001,0b00001,0b11110} },
    { 'N', {0b10001,0b11001,0b10101,0b10101,0b10011,0b10001,0b10001} },
    { 'T', {0b11111,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100} },
    { 'O', {0b01110,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110} },
    { 'B', {0b11110,0b10001,0b10001,0b11110,0b10001,0b10001,0b11110} },
    { 'G', {0b01110,0b10001,0b10000,0b10111,0b10001,0b10001,0b01111} },
    { 'I', {0b01110,0b00100,0b00100,0b00100,0b00100,0b00100,0b01110} },
    { ' ', {0,0,0,0,0,0,0} },
};

#endif // TETRIS_H
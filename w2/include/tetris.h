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
#include "tui.h"
#include "types.h"

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

typedef struct {
    unsigned char cells[HEIGHT][WIDTH];
} tetris_board_t;

typedef struct {
    unsigned char tiles[6][9][9];
} tetris_splashscreen_t;

void tetris_init(void);
void tetris_board_init(tetris_board_t *board);
bool tetris_board_full(const tetris_board_t *board, unsigned int y);
void tetris_board_clear(tetris_board_t *board, unsigned int y);
u32int tetris_board_clear_lines(tetris_board_t *board);


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

#endif // TETRIS_H
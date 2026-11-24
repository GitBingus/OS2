#include "tetris.h"

#define SPLASH_LETTERS 6
#define SPLASH_WIDTH   9
#define SPLASH_HEIGHT  9

void show_splashscreen(void) {
    
    unsigned int screen_width = 80;
    unsigned int screen_height = 25;
    
    // Calculate the starting position.
    unsigned int start_x = (screen_width - (SPLASH_LETTERS * SPLASH_WIDTH)) / 2;
    unsigned int start_y = (screen_height - SPLASH_HEIGHT) / 2;
    
    for (int letter = 0; letter < SPLASH_LETTERS; letter++) {
        
        for (int y = 0; y < SPLASH_HEIGHT; y++) {
            
            for (int x = 0; x < SPLASH_WIDTH; x++) {
                
                if (tetris_splashscreen.tiles[letter][y][x]) {
                    
                    unsigned int screen_x = start_x + (letter * SPLASH_WIDTH) + x;
                    unsigned int screen_y = start_y + y;
                    
                    unsigned int cursor = (screen_y * screen_width) + screen_x;
                    
                    fb_write_cell(cursor, 219, COLOUR.CYAN, COLOUR.BLACK);
                }
            }
        }
    }
}

static void tetris_wait_for_start(void) {
    keyboard_event_t event;
    
    while (1) {
        // Check whether a keyboard event is available.
        if (!keyboard_dequeue(&event))
        continue;
        
        // Start when Enter is pressed.
        if (event.key == KEY_ENTER)
        break;
    }
}

void show_play_prompt(void) {
    const char *prompt = "PRESS ENTER TO PLAY";
    unsigned int length = 19;
    
    unsigned int x = (SCREEN_WIDTH - length) / 2;
    unsigned int y = 22;
    
    unsigned int position = (y * SCREEN_WIDTH) + x;
    
    // Draw the prompt.
    for (unsigned int i = 0; i < length; i++) {
        fb_write_cell(
            position + i,
            prompt[i],
            COLOUR.WHITE,
            COLOUR.BLACK
        );
    }
}

void tetris_board_clear(tetris_board_t *board, unsigned int y) {
    if (y >= HEIGHT)
    return;
    
    for (unsigned int x = 0; x < WIDTH; x++) {
        board->cells[y][x] = EMPTY_CELL;
    }
}

// checks the entire board, finds completed rows, removes them and shifts the rows above down
static u32int tetris_board_clear_lines(tetris_board_t *board) {
    u32int lines_cleared = 0;
    int y = HEIGHT - 1;

    while (y >= 0) {
        if (tetris_board_full(board, y)) {

            // Shift all rows above y down by one.
            for (int yy = y; yy > 0; yy--) {
                for (u32int x = 0; x < WIDTH; x++) {
                    board->cells[yy][x] = board->cells[yy - 1][x];
                }
            }

            // Clear the top row.
            tetris_board_clear(board, 0);
            lines_cleared++;

        } else {
            y--;
        }
    }

    return lines_cleared;
}


// checks whether one specific row is completely occupied
bool tetris_board_full(const tetris_board_t *board, unsigned int y) {
    if (y >= HEIGHT)
    return false;
    
    for (unsigned int x = 0; x < WIDTH; x++) {
        int currentCell = board->cells[y][x];
        
        if (currentCell == EMPTY_CELL) {
            return false;
        }
    }
    
    return true; // if the whole row is full
}

void tetris_board_init(tetris_board_t *board) {
    for (unsigned int y = 0; y < HEIGHT; y++) {
        tetris_board_clear(board, y);
    }
}

void tetris_init(void) {
    fb_cursor_disable();
    fb_clear(FB_BLACK);
    
    show_splashscreen();
    
    show_play_prompt();
    tetris_wait_for_start();
    
    tetris_board_t Board;
    
    tetris_board_init(&Board);
}
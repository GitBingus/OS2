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
u32int tetris_board_clear_lines(tetris_board_t *board) {
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
        for (unsigned int x = 0; x < WIDTH; x++) {
            board->cells[y][x] = EMPTY_CELL;
        }
    }
}

void tetris_board_render(const tetris_board_t *board) {
    /*
        Iterate through every row.
        Iterate through every column.
        Read the cell's value.
        Determine its colour.
        Calculate its framebuffer position.
        Draw the block.
    */

    (void)(board);

    for (int y = 0; y < HEIGHT; y++) {
        for (u32int x = 0; x < WIDTH; x++) {
            unsigned int screen_x = BOARD_START_X + (x * 2);
            unsigned int screen_y = BOARD_START_Y + y;

            unsigned int cursor = (screen_y * SCREEN_WIDTH) + screen_x;
            unsigned char colour = board->cells[y][x];

            if (colour == EMPTY_CELL) {
                fb_write_cell(cursor,     ' ', COLOUR.WHITE, COLOUR.BLACK);
                fb_write_cell(cursor + 1, ' ', COLOUR.WHITE, COLOUR.BLACK);
            } else {
                fb_write_cell(cursor,     0xFE, colour, COLOUR.BLACK);
                fb_write_cell(cursor + 1, 0xFE, colour, COLOUR.BLACK);
            }
        }
    }
}

void tetris_board_render_border(void) {
    unsigned int left   = BOARD_START_X - 1;
    unsigned int right  = BOARD_START_X + BOARD_SCREEN_WIDTH;
    unsigned int top    = BOARD_START_Y - 1;
    unsigned int bottom = BOARD_START_Y + HEIGHT;

    // draw horizontal lines
    for (unsigned int x = left + 1; x < right; x++) {
        fb_write_cell((top * SCREEN_WIDTH) + x, 196, COLOUR.WHITE, COLOUR.BLACK);
        fb_write_cell((bottom * SCREEN_WIDTH) + x, 196, COLOUR.WHITE, COLOUR.BLACK);
    }

    // draw vertical lines
    for (unsigned int y = top + 1; y < bottom; y++) {
        fb_write_cell((y * SCREEN_WIDTH) + left, 179, COLOUR.WHITE, COLOUR.BLACK);
        fb_write_cell((y * SCREEN_WIDTH) + right, 179, COLOUR.WHITE, COLOUR.BLACK);
    }

    // draw corners
    fb_write_cell((top * SCREEN_WIDTH) + left, 218, COLOUR.WHITE, COLOUR.BLACK);
    fb_write_cell((top * SCREEN_WIDTH) + right, 191, COLOUR.WHITE, COLOUR.BLACK);
    fb_write_cell((bottom * SCREEN_WIDTH) + left, 192, COLOUR.WHITE, COLOUR.BLACK);
    fb_write_cell((bottom * SCREEN_WIDTH) + right, 217, COLOUR.WHITE, COLOUR.BLACK);
}


void tetris_piece_render(const tetris_piece_t *piece) {
    const tetromino_t *ttm = &tetrominoes[piece->type];
    unsigned char colour = ttm->colour;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {

            unsigned char tile = ttm->tiles[y][x];

            if (tile == 1) {
                unsigned int screen_x =
                    BOARD_START_X + ((piece->x + x) * 2);

                unsigned int screen_y =
                    BOARD_START_Y + piece->y + y;

                unsigned int cursor =
                    (screen_y * SCREEN_WIDTH) + screen_x;

                fb_write_cell(cursor,     0xFE, colour, COLOUR.BLACK);
                fb_write_cell(cursor + 1, 0xFE, colour, COLOUR.BLACK);
            }
        }
    }
}


void tetris_board_load_mosaic(tetris_board_t *board) {
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            board->cells[y][x] = TETRIS_MOSAIC[y][x];
        }
    }
}


void tetris_init(void) {
    fb_cursor_disable();
    fb_clear(FB_BLACK);
    
    show_splashscreen();
    
    show_play_prompt();
    tetris_wait_for_start();

    fb_clear(FB_BLACK);

    tetris_board_t Board;

    tui_write("Generating mosaic...");
    tetris_board_init(&Board);

    fb_clear(FB_BLACK);
    tetris_board_load_mosaic(&Board);

    tetris_board_render(&Board);
    tetris_board_render_border();

    while (1) {
        
    }
}
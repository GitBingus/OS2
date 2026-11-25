#include "tetris.h"
#include "display.h"

#define SPLASH_LETTERS 6
#define SPLASH_WIDTH   9
#define SPLASH_HEIGHT  9

static const u32int tetris_palette[16] = {   /* standard VGA colours */
    RGB(0,0,0),       RGB(0,0,170),     RGB(0,170,0),     RGB(0,170,170),
    RGB(170,0,0),     RGB(170,0,170),   RGB(170,85,0),    RGB(170,170,170),
    RGB(85,85,85),    RGB(85,85,255),   RGB(85,255,85),   RGB(85,255,255),
    RGB(255,85,85),   RGB(255,85,255),  RGB(255,255,85),  RGB(255,255,255)
};

static u32int cell_size, board_x, board_y;

static void tetris_layout(void) {
    u32int w = graphics_get_width();
    u32int h = graphics_get_height();

    cell_size = (h - 40) / HEIGHT;                  /* leave a margin top and bottom */
    board_x   = (w - WIDTH * cell_size) / 2;
    board_y   = (h - HEIGHT * cell_size) / 2;
}

static void tetris_draw_cell(int cx, int cy, unsigned char colour_index) {
    if (cx < 0 || cx >= WIDTH || cy < 0 || cy >= HEIGHT) return;   /* piece partly above the board */

    u32int x = board_x + (u32int)cx * cell_size;
    u32int y = board_y + (u32int)cy * cell_size;

    u32int colour = (colour_index == EMPTY_CELL)
                  ? RGB(20, 20, 20)
                  : tetris_palette[colour_index & 0x0F];

    display_draw_rect(x + 1, y + 1, cell_size - 2, cell_size - 2, colour);   /* 1px gap = grid */
}

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
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            tetris_draw_cell(x, y, board->cells[y][x]);
}


void tetris_board_render_border(void) {
    u32int w = WIDTH * cell_size;
    u32int h = HEIGHT * cell_size;
    u32int white = RGB(255, 255, 255);

    display_draw_rect(board_x - 3, board_y - 3, w + 6, 2, white);       /* top */
    display_draw_rect(board_x - 3, board_y + h + 1, w + 6, 2, white);   /* bottom */
    display_draw_rect(board_x - 3, board_y - 3, 2, h + 6, white);       /* left */
    display_draw_rect(board_x + w + 1, board_y - 3, 2, h + 6, white);   /* right */
}


void tetris_piece_render(const tetris_piece_t *piece) {
    const tetromino_t *ttm = &tetrominoes[piece->type];

    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            if (ttm->tiles[y][x] == 1)
                tetris_draw_cell((int)piece->x + x, (int)piece->y + y, ttm->colour);
}


void tetris_board_load_mosaic(tetris_board_t *board) {
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            board->cells[y][x] = TETRIS_MOSAIC[y][x];
        }
    }
}


void tetris_init(void) {
    display_clear();
    display_write("TETRIS\n\nPress Enter to play\n");   /* temporary splash; the VGA one needs porting */
    tetris_wait_for_start();

    display_clear();

    tetris_board_t board;
    tetris_board_init(&board);
    tetris_board_load_mosaic(&board);

    tetris_layout();
    tetris_board_render_border();
    tetris_board_render(&board);

    /* Placeholder until the game loop exists: Backspace returns to the shell */
    keyboard_event_t event;
    for (;;) {
        if (keyboard_dequeue(&event) && event.key == KEY_BACKSPACE) break;
    }

    display_clear();
}
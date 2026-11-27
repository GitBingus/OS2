#include "tetris.h"
#include "display.h"
#include "pit.h"
#include "random.h"
#include "pmm.h"

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
static u32int tetris_lighten(u32int c, u32int percent);
static u32int tetris_darken(u32int c, u32int percent);
static void tetris_draw_block(u32int x, u32int y, u32int size, u32int colour);
static bool tetris_try_move(const tetris_board_t *b, tetris_piece_t *p, int dx, int dy);

static void tetris_layout(void) {
    u32int w = graphics_get_width();
    u32int h = graphics_get_height();

    cell_size = (h - 40) / HEIGHT;                  /* leave a margin top and bottom */
    board_x   = (w - WIDTH * cell_size) / 2;
    board_y   = (h - HEIGHT * cell_size) / 2;
}

static void tetris_draw_cell(int cx, int cy, unsigned char colour_index) {
    if (cx < 0 || cx >= WIDTH || cy < 0 || cy >= HEIGHT) return;

    // cx is the column index (0 to WIDTH-1), cy is the row index (0 to HEIGHT-1)
    // cy is the vertical position, so we need to invert it to draw from the top of the screen
    // colour_index is the index into the tetris_palette array, where 0 is empty and 1-15 are the colours of the tetrominoes

    u32int x = board_x + (u32int)cx * cell_size;
    u32int y = board_y + (u32int)cy * cell_size;

    if (colour_index == EMPTY_CELL) {
        display_draw_rect(x + 1, y + 1, cell_size - 2, cell_size - 2, RGB(20, 20, 20));
        return;
    }

    tetris_draw_block(x + 1, y + 1, cell_size - 2, tetris_palette[colour_index & 0x0F]);
}

static void tetris_draw_block(u32int x, u32int y, u32int size, u32int colour) {
    u32int bevel = size / 8;
    if (bevel < 2) bevel = 2;

    u32int light = tetris_lighten(colour, 45);
    u32int dark  = tetris_darken(colour, 45);

    display_draw_rect(x, y, size, size, colour);                          /* face */

    display_draw_rect(x, y, size, bevel, light);                          /* top highlight */
    display_draw_rect(x, y, bevel, size, light);                          /* left highlight */

    display_draw_rect(x, y + size - bevel, size, bevel, dark);            /* bottom shadow */
    display_draw_rect(x + size - bevel, y, bevel, size, dark);            /* right shadow */
}

static u32int tetris_lighten(u32int c, u32int percent) {
    u32int r = RGB_RED(c)   + (255 - RGB_RED(c))   * percent / 100;
    u32int g = RGB_GREEN(c) + (255 - RGB_GREEN(c)) * percent / 100;
    u32int b = RGB_BLUE(c)  + (255 - RGB_BLUE(c))  * percent / 100;
    return RGB(r, g, b);
}

/* Blend a colour towards black (percent 0-100) */
static u32int tetris_darken(u32int c, u32int percent) {
    u32int r = RGB_RED(c)   * (100 - percent) / 100;
    u32int g = RGB_GREEN(c) * (100 - percent) / 100;
    u32int b = RGB_BLUE(c)  * (100 - percent) / 100;
    return RGB(r, g, b);
}

void show_splashscreen(void) {
    const tetris_splashscreen_t *ss = &tetris_splashscreen;   /* pointer, so no struct copy */

    const u32int letters = 6;
    const u32int rows    = 9;
    const u32int cols    = 9;
    const u32int gap     = 1;                                 /* blank columns between letters */
    const u32int total_cols = letters * cols + (letters - 1) * gap;   /* 59 */

    u32int screen_w = graphics_get_width();
    u32int screen_h = graphics_get_height();

    u32int cell = screen_w / (total_cols + 5);                /* a few columns of margin */
    if (cell < 3) cell = 3;                                   /* tetris_draw_block needs size - 2 > 0 */

    u32int origin_x = (screen_w - total_cols * cell) / 2;
    u32int origin_y = (screen_h - rows * cell) / 2;

    for (u32int letter = 0; letter < letters; letter++) {
        u32int colour = tetris_palette[(u32int)rand() % 15 + 1];   /* one colour per letter, never black */

        for (u32int row = 0; row < rows; row++) {
            for (u32int col = 0; col < cols; col++) {
                u32int px = origin_x + (letter * (cols + gap) + col) * cell;
                u32int py = origin_y + row * cell;

                if (ss->tiles[letter][row][col] == 1) {
                    tetris_draw_block(px + 1, py + 1, cell - 2, colour);
                } else {
                    display_draw_rect(px + 1, py + 1, cell - 2, cell - 2, RGB(20, 20, 20));
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


void tetris_piece_render(tetris_piece_t *piece) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (!piece->tiles[y][x]) continue;      /* empty part of the 4x4: skip */
            tetris_draw_cell(piece->x + x, piece->y + y, tetrominoes[piece->type].colour);
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

bool tetris_piece_fits(const tetris_board_t *board, const tetris_piece_t *piece) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int bx = piece->x + x;
            int by = piece->y + y;

            if (!piece->tiles[y][x]) continue;
            if (bx < 0 || bx >= WIDTH) return false;    /* side walls */
            if (by >= HEIGHT) return false;             /* floor */
            if (by >= 0 && board->cells[by][bx] != EMPTY_CELL) return false;   /* other blocks */
        }
    }

    return true;
}

static bool tetris_try_move(const tetris_board_t *b, tetris_piece_t *p, int dx, int dy) {
    tetris_piece_t moved = *p;
    moved.x += dx;
    moved.y += dy;
    if (!tetris_piece_fits(b, &moved)) return false;
    *p = moved;
    return true;
}

static void tetris_rotate_ttm(const u8int src[4][4], u8int dst[4][4]) {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            dst[x][3 - y] = src[y][x];
}

static void tetris_piece_init(tetris_piece_t *p, int type) {
    p->type = type; // set the type of the piece
    p->x = 3;       // start in the middle of the board
    p->y = 0;       // start at the top of the board

    // copy the tiles from the tetrominoes array to the piece's tiles
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            p->tiles[y][x] = tetrominoes[type].tiles[y][x];
}


void tetris_init(void) {
    display_clear();
    show_splashscreen();
    tui_write("TETRIS\n\n\n\nPRESS ENTER TO PLAY");
    tetris_wait_for_start();

    display_clear();

    tetris_board_t board;
    tetris_board_init(&board);
    // tetris_board_load_mosaic(&board); // loads test pattern

    tetris_layout();
    tetris_board_render_border();
    tetris_board_render(&board);

    /* Placeholder until the game loop exists: Backspace returns to the shell */
    keyboard_event_t event;
    bool running = true;
    bool dirty = true; /* true so the first frame draws */

    u32int drop_interval = 50;                       /* ticks between drops; lower = faster */
    u32int last_drop = pit_get_ticks();

    srand(pit_get_ticks());

    tetris_piece_t piece;
    tetris_piece_init(&piece, rand_range(0, 6));   /* spawn a random piece */

    // Game loop
    while (running) {
        while (keyboard_dequeue(&event)) {
            switch (event.key) {
                case KEY_LEFT:  if (tetris_try_move(&board, &piece, -1, 0)) dirty = true; break;
                case KEY_RIGHT: if (tetris_try_move(&board, &piece, +1, 0)) dirty = true; break;
                case KEY_DOWN:  if (tetris_try_move(&board, &piece,  0, +1)) dirty = true; break;
                case KEY_UP: {
                    tetris_piece_t rotated = piece;                      /* copies the embedded grid too */
                    tetris_rotate_ttm(piece.tiles, rotated.tiles);       /* src = current, dst = candidate */

                    if (tetris_piece_fits(&board, &rotated)) {
                        piece = rotated;                                 
                        dirty = true;
                    }
                    break;
                }

                case KEY_BACKSPACE: running = false; break;
                default: break;
            }
        }

        if ((u32int)(pit_get_ticks() - last_drop) >= drop_interval) {
            last_drop = pit_get_ticks();

            // Try to move the piece down. If it can't move down, lock it in place and spawn a new piece.
            if (tetris_try_move(&board, &piece, 0, +1)) {
                dirty = true;

            } else {
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 4; x++) {
                        if (!piece.tiles[y][x]) continue;
                        int bx = piece.x + x;
                        int by = piece.y + y;
                        if (by >= 0 && by < HEIGHT && bx >= 0 && bx < WIDTH) {
                            board.cells[by][bx] = tetrominoes[piece.type].colour;
                        }
                    }
                }
                
                tetris_board_clear_lines(&board);
                tetris_piece_init(&piece, rand_range(0, 6));  /* spawn a new random piece */
                dirty = true;

                if (!tetris_piece_fits(&board, &piece)) {
                    /* game over: stack reached the top */
                    running = false;                     /* placeholder for now */
                }
            }
        }

        if (dirty) {
            tetris_board_render(&board);     /* wipes the old piece */
            tetris_piece_render(&piece);     /* draws it at the new position */
            dirty = false;
        }

        __asm__ volatile("hlt"); // halt until the next interrupt (keyboard, timer, etc.)
    }

    // if it reaches here, the game is over -- return to shell
}
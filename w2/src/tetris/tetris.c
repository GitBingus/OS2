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

void tetris_board_clear(tetris_board_t *board, unsigned int y){
    for (int xx = 0; xx <= WIDTH; xx++) {
        board->cells[xx][y] = EMPTY_CELL;
    }
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
#ifndef TETROMINOES_H
#define TETROMINOES_H


#include "fb.h"

typedef enum {
    TETROMINO_TYPE_I,
    TETROMINO_TYPE_O,
    TETROMINO_TYPE_T,
    TETROMINO_TYPE_S,
    TETROMINO_TYPE_Z,
    TETROMINO_TYPE_J,
    TETROMINO_TYPE_L
} tetromino_type_t;


typedef struct {
    unsigned char colour;
    unsigned char tiles[4][4];
} tetromino_t;


static const tetromino_t TETROMINO_O = {
    .colour      =  COLOUR.YELLOW,
    .tiles       =  {
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}
                    }
};

static const tetromino_t TETROMINO_I = {
    .colour      =  COLOUR.RED,
    .tiles       =  {
                        {1, 0, 0, 0},
                        {1, 0, 0, 0},
                        {1, 0, 0, 0},
                        {1, 0, 0, 0}
                    }
};

static const tetromino_t TETROMINO_S = {
    .colour      =  COLOUR.RED,
    .tiles       =  {
                        {0, 1, 1, 0},
                        {1, 1, 0, 0},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}
                    }
};

static const tetromino_t TETROMINO_Z = {
    .colour      =  COLOUR.GREEN,
    .tiles       =  {
                        {1, 1, 0, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}
                    }
};

static const tetromino_t TETROMINO_L = {
    .colour      =  COLOUR.BROWN,
    .tiles       =  {
                        {1, 0, 0, 0},
                        {1, 0, 0, 0},
                        {1, 1, 0, 0},
                        {0, 0, 0, 0}
                    }
};

static const tetromino_t TETROMINO_J = {
    .colour      =  COLOUR.LIGHT_MAGENTA,
    .tiles       =  {
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {1, 1, 0, 0},
                        {0, 0, 0, 0}
                    }
};

static const tetromino_t TETROMINO_T = {
    .colour      =  COLOUR.MAGENTA,
    .tiles       =  {
                        {1, 1, 1, 0},
                        {0, 1, 0, 0},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}
                    }
};


static const tetromino_t tetrominoes[7] = {
    [TETROMINO_TYPE_I] = TETROMINO_I,
    [TETROMINO_TYPE_O] = TETROMINO_O,
    [TETROMINO_TYPE_T] = TETROMINO_T,
    [TETROMINO_TYPE_S] = TETROMINO_S,
    [TETROMINO_TYPE_Z] = TETROMINO_Z,
    [TETROMINO_TYPE_J] = TETROMINO_J,
    [TETROMINO_TYPE_L] = TETROMINO_L
};

#endif
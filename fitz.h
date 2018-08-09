#ifndef FITZ_H
#define FITZ_H

#define MAX_BUFFER 70
#define TILE_MAX_ROW 5
#define TILE_MAX_COL 5
#define TILE_SIZE 25

// program error codes
typedef enum {
    OK = 0,
    E_ARGS = 1,
    E_TFILE_IO = 2,
    E_TFILE_R = 3,
    E_PLAYER = 4,
    E_DIM = 5,
    E_SFILE_IO = 6,
    E_SFILE_R = 7,
    E_EOF = 10,
    UTIL = 42
} Err;

// instance of game
typedef struct {
    char p1type;
    char p2type;
    int dims[2]; // stores dimension of game as {row, col}
    int tileCount;
    int nextTile;
    int nextPlayer;
    char *board; // current instance of the board
    char **tiles; // loaded tiles
} Game;

#endif

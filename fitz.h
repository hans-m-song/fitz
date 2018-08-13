#ifndef FITZ_H
#define FITZ_H

#define MAX_BUFFER 70

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
    UTIL = 42,
    SAVE = 43,
    SUCCESS = 44,
    FAIL = 45
} Err;

// instance of a move
typedef struct {
    int r;
    int c;
    int theta;
    char file[MAX_BUFFER];
} Move;

// instance of game
typedef struct {
    char playerType[2];
    int dims[2]; // stores dimension of game as {row, col}
    Move moves[2]; // stores the last valid move of either player
    int tileCount;
    int nextTile;
    int nextPlayer;
    char* board; // current instance of the board
    char** tiles; // loaded tiles
} Game;


#endif

#ifndef FITZ_H
#define FITZ_H

#include <stdio.h>
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
    SAVE_FAIL = 44,
    SUCCESS = 45,
    FAIL = 46
} Err;

// instance of a move
typedef struct {
    int r;
    int c;
    int theta; // stores the number of rotations (0:0, 1:90, 2:180, 3:270)
} Move;

// instance of game
typedef struct {
    char playerType[2];
    int dims[2]; // stores dimension of game as {row, col}
    Move moves[2]; // stores the last valid move of either player
    int tileCount;
    int moveCount;
    int nextTile;
    int nextPlayer;
    char* board; // current instance of the board
    char** tiles; // loaded tiles
} Game;

void print_board(char* board, int dims[2], FILE* f);

#endif

#ifndef FITZ_H
#define FITZ_H

#define MAX_BUFF 70
#define TILE_SIZE 30

typedef enum {
    OK = 0,
    E_ARGS = 1,
    E_TFILE_IO = 2,
    E_TFILE_R = 3,
    E_PLAYER = 4,
    E_DIM = 5,
    E_SFILE_IO = 6,
    E_SFILE_R = 7,
    E_EOF = 10
} Err;

typedef struct {
    char p1type;
    char p2type;
    int dims[2];
    int numMoves;
    int numTiles;
    char **board;
    char **tiles;
} Game;

#endif

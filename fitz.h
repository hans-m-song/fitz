#ifndef FITZ_H
#define FITZ_H

#define MAX_BUFF 70

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
    FILE *tfile;
    FILE *sfile;
    char p1;
    char p2;
    int dims[2];
} Game;

#endif

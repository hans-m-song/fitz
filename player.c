#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tile.h"
#include "fitz.h"

// checks if the given tile fits on the board at the given row, column
// and rotation (in degrees)
// maps tile to coordinates relative to board starting from top left corner 
// and checks for overhangs and empty space {r - 2, c - 2} until {r + 2, c + 2}
// returns SUCCESS if valid, otherwise returns FAIL
int valid_move(Game* g, int r, int c, int theta) {
#ifdef VERBOSE
    fprintf(stdout, "checking move %d %d %d: ", r, c, theta);
#endif

    if(r < -2 || r > g->dims[0] + 2 || c < -2 || c > g->dims[1] + 2 || 
            theta < 0 || theta % 90 != 0 || theta > 270) { // check bounds
#ifdef VERBOSE
        printf("out of bounds\n");
#endif
        return FAIL;
    }

    char tile[TILE_SIZE + 1];
    rotate(theta / 90, g->tiles[g->nextTile], tile); // rotate if neccessary
    
    int rowMin = r - 2; // find the corner of the tile with respect to board
    int colMin = c - 2;

    int i, j, tileRow, tileCol; // overlay tile on board
    for(i = rowMin, tileRow = 0; tileRow < TILE_MAX_COL; i++, tileRow++) {
        for(j = colMin, tileCol = 0; tileCol < TILE_MAX_COL; j++, tileCol++) {
            // check if tile hangs over edge of board
            if(tile[tileRow * TILE_MAX_COL + tileCol] == '!' && 
                    (i < 0 || i > g->dims[0] ||
                    j < 0 || j > g->dims[1])) {
#ifdef VERBOSE
                printf("overhangs %d %d\n", i, j);
#endif
                return FAIL;
            }

            // check if space is empty
            if(tile[tileRow * TILE_MAX_COL + tileCol] == '!' &&
                    g->board[i * g->dims[1] + j] != '.') { 
#ifdef VERBOSE
                printf("overlaps\n");
#endif
                return FAIL;
            }
        }
    }
    return SUCCESS;
}

// attempts to opens a file with the given filename
// if successful, will save information of the given game and close the file
// returns SUCCESS or SAVE_FAIL
int save_game(char* fileName, Game* g) {
    FILE* f = fopen(fileName, "w");
    if(!f) {
        return SAVE_FAIL;
    }

    fprintf(f, "%d %d %d %d\n", g->nextTile, g->nextPlayer, g->dims[0], 
            g->dims[1]);
    print_board(g->board, g->dims, f);
    fflush(f);
    fclose(f);
    return SUCCESS;
}

// type 1 autoplayer, takes the game instance and a move struct containing 
// in which it will save its next move to
// returns SUCCESS if valid move found, otherwise returns FAIL
int a1_move(Game* g, Move* m) {
    int r, rStart, c, cStart, theta;
    if(g->moveCount == 0) {
        r = -2;
        c = -2;
    } else { 
        r = g->moves[(g->nextPlayer + 1) % 2].r;
        c = g->moves[(g->nextPlayer + 1) % 2].c;
    }
    rStart = r;
    cStart = c;
    theta = 0;

    while(theta < 4) {
        while(1) {
            if(valid_move(g, r, c, theta) == SUCCESS) {
                m->r = r;
                m->c = c;
                m->theta = theta * 90;
                return SUCCESS;
            }

            c += 1;
            if(c > g->dims[1] + 2) {
                c = -2;
                r += 1;
            }

            if(r > g->dims[0] + 2) {
                r = -2;
            }

            if(r == rStart && c == cStart) {
                break;
            }
        }
        theta += 1;
    }

    return FAIL;
}

// type 2 autoplayer, takes the game instance and a move struct containing 
// in which it will save its next move to
// first determines which direction to move (l -> r, t -> b or r -> l, b -> t)
// then initializes start and end to the top left or bottom right appropriately
// then it will move in the direction determined earlier
// returns SUCCESS if valid move found, otherwise returns FAIL
int a2_move(Game* g, Move* m) {
    int r, rStart, c, cStart, theta;
    int start[2], end[2], direction; // for where to start and where to end
    if(g->nextPlayer == 0) { // go left to right, downwards
        direction = 1;
        memcpy(start, (int[]){-2, -2}, sizeof(start));
        memcpy(end, (int[]){g->dims[0] + 2, g->dims[1] + 2}, sizeof(end));
    } else { // go right to left, upwards
        direction = -1;
        memcpy(start, (int[]){g->dims[0] + 2, g->dims[1] + 2}, sizeof(start));
        memcpy(end, (int[]){-2, -2}, sizeof(end));
    }
    if(g->moveCount == 0) {
        r = start[0];
        c = start[1];
    } else {
        r = m->r;
        c = m->c;
    }
    rStart = r;
    cStart = c;

    while(1) {
        while(theta < 4) {
            if(valid_move(g, r, c, theta * 90) == SUCCESS) {
                m->r = r;
                m->c = c;
                m->theta = theta;
                return SUCCESS;
            }
            theta += 1;
        }
        theta = 0;
        c += 1 * direction;
        if(c == end[1] + (1 * direction)) {
            c = start[1];
            r += 1 * direction;
        }

        if(r == end[0] + (1 * direction)) {
            r = start[0];
        }

        if(r == rStart && c == cStart) {
            break;
        }
    }

    return FAIL;
}

// human move, retrieves and parses user input and saves it in the given 
// move struct
// returns an error code describing the input
int h_move(Game* g, Move* m) {
    char* str = (char*)malloc(sizeof(char) * MAX_BUFFER + 1);
    
    str = fgets(str, MAX_BUFFER, stdin);
    if(str == NULL) {
        free(str);
        return E_EOF;
    }

    Err e = FAIL;
    int r, c, theta;
    char fileName[MAX_BUFFER];
    if(sscanf(str, "%d %d %d", &r, &c, &theta) == 3) {
        if(valid_move(g, r, c, theta) == SUCCESS) {
#ifdef TEST
            fprintf(stdout, "hmove success %d, %d, %d\n", r, c, theta);
#endif

            m->r = r;
            m->c = c;
            m->theta = theta / 90;

            e = SUCCESS;
        }
    } else if(sscanf(str, "save%s\n", fileName) == 1) {
#ifdef TEST
        fprintf(stdout, "hmove saving to: %s\n", fileName);
#endif

        if(save_game(fileName, g) == SUCCESS) {
            e = SAVE;
        }
    }

    free(str);
    return e;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fitz.h"
#include "init.h"
#include "tile.h"

// frees all memory used by the given game instance
void end_game(Game* g) {
    free(g->board);
    clear_tiles(g->tiles, g->tileCount);
}

// prints the 2D representation of the board in the given dimensions for the 
// given 1D representation of the board to the given output
void print_board(char* board, int dims[2], FILE* f) {
    int i, j;
    for(i = 0; i < dims[0]; i++) {
        for (j = 0; j < dims[1]; j++) {
            fprintf(f, "%c", board[(i * dims[1]) + j]);
        }
        fprintf(f, "\n");
    }
    fflush(f);
}

// saves game instance to the given file
// assumes file is valid
int save_game(char* fileName, Game* g) {
    FILE* f = fopen(fileName, "w");
    if(!f) {
        return FAIL;
    }

    fprintf(f, "%d %d %d %d\n", g->nextTile, g->nextPlayer, g->dims[0], 
            g->dims[1]);
    print_board(g->board, g->dims, f);
    fflush(f);
    fclose(f);
    return SUCCESS;
}

// takes the board and applys the tile with the given move
// assumes move is valid
void do_move(Game* g, char pSymbol) {
    int rowMin = g->moves[g->nextPlayer].r - 2; // get corner of tile with
    int colMin = g->moves[g->nextPlayer].c - 2; // respect to board

    char tile[TILE_SIZE + 1]; // get rotated tile
    rotate(g->moves[g->nextPlayer].theta, g->tiles[g->nextTile], tile); 

    int i, j, tileRow, tileCol; // iterate board and tile simultaneously
    for(i = rowMin, tileRow = 0; tileRow < TILE_MAX_ROW; i++, tileRow++) {
        for(j = colMin, tileCol = 0; tileCol < TILE_MAX_COL; j++, tileCol++) {
            if(tile[tileRow * TILE_MAX_COL + tileCol] == '!') {
#ifdef TEST
                printf("%d, %d\n", i, j);
#endif
                g->board[i * g->dims[1] + j] = pSymbol;
            }
        }
    }
}

// checks if the given tile fits on the board at the given row, column
// and rotation
// returns 1 if valid, otherwise returns 0
int valid_move(Game* g, int r, int c, int theta) {
    if(r < -2 || r > g->dims[0] + 2 || c < -2 || c > g->dims[1] + 3 || 
            theta % 90 != 0 || theta < 0 || theta > 270) {
        return FAIL;
    }

    char tile[TILE_SIZE + 1];
    rotate(theta / 90, g->tiles[g->nextTile], tile);
    
    int rowMin = r - 2; // find the corner of the tile with respect to board
    int colMin = c - 2;

    int i, j, tileRow, tileCol;
    for(i = rowMin, tileRow = 0; tileRow < TILE_MAX_COL; i++, tileRow++) {
        for(j = colMin, tileCol = 0; tileCol < TILE_MAX_COL; j++, tileCol++) {
            if(tile[tileRow * TILE_MAX_COL + tileCol] == '!' && 
                    (i < 0 || i > g->dims[0] || // check overhangs
                    j < 0 || j > g->dims[1])) {
                return FAIL;
            }
            if(tile[tileRow * TILE_MAX_COL + tileCol] == '!' && // check empty
                    g->board[i * g->dims[1] + j] != '.') {
                return FAIL;
            }
        }
    }
    return SUCCESS;
}

// type 1 autoplayer, takes the game instance and a move struct containing the
// previous move, saves its next move to the struct
// returns 1 if valid move found, otherwise returns 0
int a1_move(Game* g) {
    int r, c, theta;
    if(g->moveCount == 0) {
        r = -2;
        c = -2;
        theta = 0;
    }

    Err e = FAIL;
    while(e == FAIL) {
        e = valid_move(g, r, c, theta);
        if(e == SUCCESS) {
            g->moves[g->nextPlayer].r = r;
            g->moves[g->nextPlayer].c = c;
            g->moves[g->nextPlayer].theta = theta;
        }
        c += 1;
        if(c > g->dims[1] + 2) {
            c = -2;
            r += 1;
        }
        if(c > g->dims[0] + 2) {
            r = -2;
        }
    }


#ifdef TEST
    printf("amove type1\n");
    return SUCCESS;
#endif
    return FAIL;
}

// type 2 autoplayer, same as a1Move
int a2_move(Game* g) {
#ifdef TEST
    printf("amove type2\n");
    return SUCCESS;
#endif
    return FAIL;
}

// human move, retrieves and parses user input and saves it in the given move
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
            m->r = r;
            m->c = c;
            m->theta = theta / 90;

            e = SUCCESS;

#ifdef TEST
            fprintf(stdout, "hmove success %d, %d, %d\n", r, c, theta);
#endif
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

// game runtime instance, plays the game in the given game instance
// returns an error code
int play_game(Game* g) {
    char pSymbol[] = {'*', '#'};

    while(1) { // TODO change this later while(!win_condition(g)) {
        print_board(g->board, g->dims, stdout);
        // check win condition win(board, tile)

        Err e = FAIL;
        if(g->playerType[g->nextPlayer] == 'h') { // get human move
            print_tile(g->tiles[g->nextTile]);
            while(e == FAIL) {
                fprintf(stdout, "Player %c] ", pSymbol[g->nextPlayer]);
                e = h_move(g, &g->moves[g->nextPlayer]);
            }
        } else { // get autoplayer move
            fprintf(stdout, "Player %c => ", pSymbol[g->nextPlayer]);
            if(g->playerType[g->nextPlayer] == '1') {
                e = a1_move(g);
            } else {
                e = a2_move(g);
            }
        } 
        
        if(e == E_EOF) { // if user quit
            return e;
        }

        if(e == SAVE) { // if user requested save
            break;
        }

        if(e == SAVE_FAIL) {
            err_msg(e);
        } else {
            do_move(g, pSymbol[g->nextPlayer]);

            // increment player and tile count
            g->nextPlayer += 1;
            g->nextPlayer %= 2;
            g->moveCount += 1;
            g->nextTile += 1;
            g->nextTile %= g->tileCount;
        }
    }
    return OK;
}

// intializes the game using the given game instance and invocation arguments
// returns an error code
int init_game(Game* g, int argc, char** argv) {
    if(check_player(argv[2]) || check_player(argv[3])) {
        return E_PLAYER;
    }
    g->playerType[0] = argv[2][0];
    g->playerType[1] = argv[3][0];

    Err e = OK;
    if(argc == 5) {
        e = check_file(g, 's', argv[4]); // load saved game
        if(e) {
            return e;
        }
    } else {
        g->nextPlayer = 0;
        g->nextTile = 0;
        g->moveCount = 0;
        
        e = check_dims(g, argv[4], argv[5]);
        if(e) {
            return e;
        }
        g->board = (char*)malloc(sizeof(char) * g->dims[0] * g->dims[1] + 1);
        memset(g->board, '.', g->dims[0] * g->dims[1]);
        g->board[g->dims[0] * g->dims[1]] = '\0';
    }
    
    init_move(g->moves[0], g->playerType[0]);
    init_move(g->moves[1], g->playerType[1]);

    return e;
}

int main(int argc, char** argv) {
    Err e = OK;
    Game g; 

    if(argc != 2 && argc != 5 && argc != 6) { // check for correct invocation
        e = E_ARGS;
        err_msg(e);
        return e;
    }

#ifdef TEST
    fprintf(stdout, "got %d args\n", argc);
#endif

    e = check_file(&g, 't', argv[1]); // check tile file
    if(e) {
        err_msg(e);
        return e;
    }
    
    if(argc == 2) {
        print_all_tiles(g.tiles, g.tileCount);
        clear_tiles(g.tiles, g.tileCount);
        return e;
    } 
    
    e = init_game(&g, argc, argv);
    if(e) {
        err_msg(e);
        clear_tiles(g.tiles, g.tileCount);
        return e;
    }
    
    e = play_game(&g);
    if(e) {
        err_msg(e);
    }

    end_game(&g);
    return e;
}

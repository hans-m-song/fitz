#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"
#include "init.h"
#include "tile.h"

// frees memory used for tiles in the given game instance
void clear_tiles(Game* g) {
    int i;
    for(i = 0; i <= g->tileCount; i++) {
        free(g->tiles[i]);
    }
    free(g->tiles);
}

// frees all memory used by the given game instance
void end_game(Game* g) {
    free(g->board);
    clear_tiles(g);
}

// prints the 2D representation of the board in the given dimensions for the 
// given 1D representation of the board to the given output
void print_board(char* board, int dims[2], FILE* f) {
    int i, j;
    for(i = 0; i < dims[0]; i++) {
        for (j = 0; j < dims[1]; j++) {
            fprintf(f, "%c", board[(i * dims[0]) + j]);
        }
        fprintf(f, "\n");
    }
}

// saves game instance to the given file
// assumes file is valid
int save_game(char* fileName, Game* g) {
    FILE* f = fopen(fileName, "w");
    if(!f) {
        return 0;
    }

    fprintf(f, "%d %d %d %d\n", g->nextTile, g->nextPlayer, g->dims[0], 
            g->dims[1]);
    print_board(g->board, g->dims, f);
    fflush(f);
    return 1;
}

// checks if the given tile fits on the board at the given row, column
// and rotation
// returns 1 if valid, otherwise returns 0
int valid_move(Game* g, int r, int c, int theta) {
    char tile[TILE_SIZE + 1];
    rotate(theta, g->tiles[g->nextTile], tile);
    
    // check if tile is bigger than board
    // check if enough space for tile (win condition? seperate function?)

    int i, j; // check if any part of tile is out of bounds
    for(i = 0; i < TILE_MAX_ROW; i++) {
        for(j = 0; j < TILE_MAX_COL; j++) {
        }
    }

    return 1;
}

// type 1 autoplayer, takes the game instance and a move struct containing the
// previous move, saves its next move to the struct
// returns 1 if valid move found, otherwise returns 0
int a1_move(Game* g, Move m) {
    #ifdef TEST
        printf("amove type1\n");
        return SUCCESS;
    #endif
    return 1;
}

// type 2 autoplayer, same as a1Move
int a2_move(Game* g, Move m) {
    #ifdef TEST
        printf("amove type2\n");
        return SUCCESS;
    #endif
    return 1;
}

// human move, retrieves and parses user input
// returns a special code on save or valid move or an error code otherwise
int h_move(Game* g, Move m) {
    char str[MAX_BUFFER + 1];
    if(!fgets(str, MAX_BUFFER + 1, stdin)) {
        return E_EOF;
    }

    int r, c, theta;
    char fileName[MAX_BUFFER];
    if(sscanf(str, "%d %d %d\n", &r, &c, &theta) == 3 && strlen(str) == 6) {
        // check move
        #ifdef TEST
            fprintf(stdout, "hmove got: %d, %d, %d\n", r, c, theta);
        #endif
        if(valid_move(g, r, c, theta)) {
            m.r = r;
            m.c = c;
            m.theta = theta;
            return SUCCESS; 
        }
    } else if(sscanf(str, "save%s\n", fileName) == 1) {
        //save
        #ifdef TEST
            fprintf(stdout, "hmove saving to: %s\n", fileName);
        #endif
        if(save_game(fileName, g)) {
            return SAVE;
        }
    }

    return OK;
}

// game runtime instance, plays the game in the given game instance
// returns an error code
int play_game(Game* g) {
    char pSymbol[] = {'*', '#'};

    int i = 0;
    while(i < 4) { // TODO change this later
        print_board(g->board, g->dims, stdout);
        // check win condition win(board, tile)
        
        Err e = OK;
        while(e == OK) { // get player move
            if(g->playerType[g->nextPlayer] == 'h') { // get human move
                print_tile(g->tiles[g->nextTile]);
                fprintf(stdout, "Player %c] ", pSymbol[g->nextPlayer]);
                e = h_move(g, g->move[g->nextPlayer]);
            } else { // get autoplayer move
                fprintf(stdout, "Player %c => ", pSymbol[g->nextPlayer]);
                if(g->playerType[g->nextPlayer] == '1') {
                    e = a1_move(g, g->move[g->nextPlayer]);
                } else {
                    e = a2_move(g, g->move[g->nextPlayer]);
                }
            } 
        
            if(e == EOF) {
                return e;
            }

            // do move
            // check for save or quit
        }

        // increment player and tile count
        g->nextPlayer += 1;
        g->nextPlayer %= 2;
        g->nextTile += 1;
        g->nextTile %= g->tileCount;
        i++;
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
        
        e = check_dims(g, argv[4], argv[5]);
        if(e) {
            return e;
        }
        g->board = (char*)malloc(sizeof(char) * g->dims[0] * g->dims[1] + 1);
        memset(g->board, '.', g->dims[0] * g->dims[1]);
        g->board[g->dims[0] * g->dims[0]] = '\0';
    }
    
    init_move(g->move[0], g->playerType[0]);
    init_move(g->move[1], g->playerType[1]);

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
        clear_tiles(&g);
        return e;
    } 
    
    e = init_game(&g, argc, argv);
    if(e) {
        err_msg(e);
        clear_tiles(&g);
        return e;
    }
    
    e = play_game(&g);
    if(e) {
        err_msg(e);
    }

    end_game(&g);
    return e;
}

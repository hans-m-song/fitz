#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fitz.h"
#include "init.h"
#include "tile.h"
#include "player.h"

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

int win_condition(Game* g) {
    // do an autoplayer move?
    return 1;
}

// takes the board and applys the tile with the given move
// assumes move is valid
void do_move(Game* g, char pSymbol) {
#ifdef TEST
    fprintf(stdout, "doing move %d %d %d\n", g->moves[g->nextPlayer].r, 
            g->moves[g->nextPlayer].c, g->moves[g->nextPlayer].theta);
#endif

    int rowMin = g->moves[g->nextPlayer].r - 2; // get corner of tile with
    int colMin = g->moves[g->nextPlayer].c - 2; // respect to board

    char tile[TILE_SIZE + 1]; // get rotated tile
    rotate(g->moves[g->nextPlayer].theta, g->tiles[g->nextTile], tile); 

    int i, j, tileRow, tileCol; // iterate board and tile simultaneously
    for(i = rowMin, tileRow = 0; tileRow < TILE_MAX_ROW; i++, tileRow++) {
        for(j = colMin, tileCol = 0; tileCol < TILE_MAX_COL; j++, tileCol++) {
            if(tile[tileRow * TILE_MAX_COL + tileCol] == '!') {
                g->board[i * g->dims[1] + j] = pSymbol;
            }
        }
    }
}

// game runtime instance, plays the game in the given game instance
// returns an error code
int play_game(Game* g) {
    char pSymbol[] = {'*', '#'};

    while(1) { // TODO change this later while(!win_condition(g)) {
        print_board(g->board, g->dims, stdout);
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
                e = a1_move(g, &g->moves[g->nextPlayer]);
            } else {
                e = a2_move(g, &g->moves[g->nextPlayer]);
            }
            fprintf(stdout, "%d %d rotated %d\n", g->moves[g->nextPlayer].r, 
                    g->moves[g->nextPlayer].c, 
                    g->moves[g->nextPlayer].theta * 90);
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

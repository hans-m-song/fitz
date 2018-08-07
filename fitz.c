#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"
#include "lib.h"

void end_game(Game *g) {
    if(g->numTiles) {
        int i;
        for(i = 0; i < g->numTiles; i++) {
            free(g->tiles[i]);
        }
        free(g->tiles);
    }
    if(g->board) {
        free(g->board);
    }
}

void print_square(char *data, int dims[2]) {
    printf("square\n");
}

int init_game(Game *g, int argc, char **argv) {
    if(check_player(argv[2]) || check_player(argv[3])) {
        return E_PLAYER;
    }
    g->p1type = argv[2][0];
    g->p2type = argv[3][0];

    Err e = OK;
    if(argc == 5) {
        e = check_file(g, 's', argv[4]); // load saved game
    } else {
        e = check_dims(g, argv[4], argv[5]);
        g->board = malloc(sizeof(char) * g->dims[0] * g->dims[1] + 1);
        memset(g->board, '.', g->dims[0] * g->dims[1]);
        g->board[g->dims[0] * g->dims[0]] = '\0';
    }

    return e;
}

int main(int argc, char **argv) {
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
        // print tiles
        printf("imagine tiles being printed here\n");
        return e;
    } else {
        e = init_game(&g, argc, argv);
    }
    if(e) {
        err_msg(e);
        return e;
    }

    // play game

    end_game(&g);
    return e;
}

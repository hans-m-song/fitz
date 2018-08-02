#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"

// takes an integer representation of an error code
// prints the corresponding error code
void err_msg(Err e) {
    switch(e) {
        case E_ARGS:
            printf("Usage: fitz tilefile [argv[2]type argv[3]type [height width | filename]]\n");
            break;
        case E_TFILE_IO:
            printf("Can't access tile file\n");
            break;
        case E_TFILE_R:
            printf("Invalid tile file contents\n");
            break;
        case E_PLAYER:
            printf("Invalid player type\n");
            break;
        case E_DIM:
            printf("Invalid dimensions\n");
            break;
        case E_SFILE_IO:
            printf("Can't access save file\n");
            break;
        case E_SFILE_R:
            printf("Invalid save file contents\n");
            break;
        case E_EOF:
            printf("End of input\n");
            break;
        default:
            break;
    }
}

// frees any allocated memory
void end_game(Game *g) {
    if(g->tiles) {
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

// loads save file data into game instance from given file
int parse_sfile(Game *g, FILE *f) {

    return OK;
}

// loads tiles into game instance from given file 
// @todo and prints them
int parse_tfile(Game *g, FILE *f) {
    /*g->tiles = malloc(sizeof(*char)*2);
    int i, temp, tiles;
    while(1) {
        for(i = 0; i < TILE_SIZE; i++) {
            temp = fgetc(f);
            if(temp == EOF || ((char)temp != ',' && (char)temp !== '!' &&
                    (char)temp != '\n')) {
                return E_TFILE_R;
            }

        }
    }*/

    return OK;
}

// takes a file pointer to a tile file and checks for correct formatting
// @todo save metadata in given struct?
// returns an error code
int check_file(Game *g, char type, char *filename) {
    FILE *f = fopen(filename, "r"); // check tilefile
    if(!f) {
        return E_TFILE_IO;
    }
    #ifdef TEST
        printf("received %c file %s\n", type, filename);
    #endif

    Err e = OK; 
    switch(type) {
        case 's':
            e = parse_sfile(g, f);
            break;
        case 't':
            e = parse_tfile(g, f);
            break;
        default:
            break;
    }
    
    fclose(f);
    return e;
}

// checks player types
int check_players(Game *g, char *p1type, char *p2type) {
    if((strcmp(p1type, "h") != 0 && strcmp(p2type, "1") != 0 && 
            strcmp(p1type, "2") != 0) || (strcmp(p2type, "h") != 0 && 
            strcmp(p2type, "1") != 0 && strcmp(p2type, "2") != 0)) {
        return E_PLAYER;
    }
    g->p1type = p1type[0];
    g->p2type = p2type[0];
    #ifdef TEST
        printf("p1: %c, p2: %c\n", g->p1type, g->p2type);
    #endif
    return OK;
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
        printf("received %d args\n", argc);
    #endif
    e = check_file(&g, 't', argv[1]); // check tile file
    if(e) {
        err_msg(e);
        return e;
    }
    
    if(argc == 2) {
        // print tiles
    } else {
        e = check_players(&g, argv[2], argv[3]); // check player types
        if(e) {
            err_msg(e);
            return e;
        }
        if(argc == 5) {
            e = check_file(&g, 's', argv[4]); // load saved game
            if(e) {
                err_msg(e);
                return e;
            }
        } else {
            // initialize new game
        }
    }

    end_game(&g);
    return e;
}

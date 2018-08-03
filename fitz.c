#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"

// takes an integer representation of an error code
// prints the corresponding error code
void err_msg(Err e) {
            char *str;
    switch(e) {
        case E_ARGS:
            str = "Usage: fitz tilefile"
                  " [p1type p2type [height width | filename]]\n";
            break;
        case E_TFILE_IO:
            str = "Can't access tile file\n";
            break;
        case E_TFILE_R:
            str = "Invalid tile file contents\n";
            break;
        case E_PLAYER:
            str = "Invalid player type\n";
            break;
        case E_DIM:
            str = "Invalid dimensions\n";
            break;
        case E_SFILE_IO:
            str = "Can't access save file\n";
            break;
        case E_SFILE_R:
            str = "Invalid save file contents\n";
            break;
        case E_EOF:
            str = "End of input\n";
            break;
        default:
            break;
    }
    fprintf(stderr, str);
    fflush(stderr);
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
        fprintf(stdout, "received %c file %s\n", type, filename);
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

// checks player type of given string
// returns an error code
int check_player(char *ptype) {
    if(strlen(ptype) != 1 ||
            (ptype[0] != '1' && ptype[0] != '2' && ptype[0] != 'h')) {
        return E_PLAYER;
    }
    #ifdef TEST
        fprintf(stdout, "received players:%c,%c\n", g->p1type, g->p2type);
    #endif
    return OK;
}

// checks dimensions of given strings and saves them to given game instance
// returns an error code
int check_dims(Game *g, char *row, char *col) {
    char *temp;
    long int x = strtol(row, &temp, 10);
    long int y = strtol(col, &temp, 10);
    if(!x || !y || (x < 1 || x > 999) || (y < 1 || y > 999)) {
        return E_DIM;
    }
    g->dims[0] = (int)x;
    g->dims[1] = (int)y;
    #ifdef TEST
        fprintf(stdout, "received dimensions:%d,%d\n", g->dims[0], g->dims[1]);
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
        fprintf(stdout, "received %d args\n", argc);
    #endif
    e = check_file(&g, 't', argv[1]); // check tile file
    if(e) {
        err_msg(e);
        return e;
    }
    
    if(argc == 2) {
        // print tiles
        return e;
    } else {
        if(check_player(argv[2]) || check_player(argv[3])) {
            e = E_PLAYER;
            err_msg(e);
            return e;
        }
        g.p1type = argv[2][0];
        g.p2type = argv[3][0];

        if(argc == 5) {
            e = check_file(&g, 's', argv[4]); // load saved game
            if(e) {
                err_msg(e);
                return e;
            }
        } else {
            e = check_dims(&g, argv[4], argv[5]);
            if(e) {
                err_msg(e);
                return e;
            }
        }
    }

    // end_game(&g);
    return e;
}

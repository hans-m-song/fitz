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
            printf("Can’t access tile file\n");
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

// frees any allocated memory and closes all files
void end_game(Game *g) {
    fclose(g->tfile);
    fclose(g->sfile);
}

// takes a file pointer to a tile file and checks for correct formatting
// @todo save metadata in given struct?
// returns an error code
int check_tfile(FILE *tfile) {
    Err e = OK;
    // e = E_TFILE_R;
    // fclose(tfile);
    return e;
}

// takes a file pointer to a save file and checks for correct formatting
// returns an error code
int check_sfile(FILE *sfile) {
    return 0;
}

int main(int argc, char **argv) {
    Err e = OK;
    Game g; 
    if(argc != 2 && argc != 5 && argc != 6) { // check for correct invocation
        e = E_ARGS;
        err_msg(e);
        return e;
    }
    
    g.tfile = fopen(argv[1], "r"); // check tilefile
    if(!g.tfile) {
        e = E_TFILE_IO;
        err_msg(e);
        return e;
    }
    e = check_tfile(g.tfile);
    if(e) {
        err_msg(e);
        return e;
    }
    
    if(argc != 2) {
        // checks player types
        if((strcmp(argv[2], "h") != 0 && strcmp(argv[2], "1") != 0 && 
                strcmp(argv[2], "2") != 0) || (strcmp(argv[3], "h") != 0 && 
                strcmp(argv[3], "1") != 0 && strcmp(argv[3], "2") != 0)) {
            e = E_PLAYER;
            err_msg(e);
            return e;
        }
        g.p1 = argv[2][0];
        g.p2 = argv[3][0];
        
    } else {
        g.p1 = 'h';
        g.p2 = 'h';
    }

    //end_game(&g);
    return e;
}

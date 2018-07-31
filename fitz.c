#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"

void err_msg(Err e) {
    switch(e) {
        case E_ARGS:
            printf("Usage: fitz tilefile [p1type p2type [height width | filename]]\n");
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

int check_tilefile(FILE *tf) {
    Err e = OK;
    // e = E_TFILE_R;
    return e;
}

int main(int argc, char **argv) {
    Err e = OK;
    
    if(argc != 2 && argc != 5 && argc != 6) { // check args
        e = E_ARGS;
        err_msg(e);
        return e;
    }
    
    FILE *tf = fopen(argv[1], "r"); // check tilefile
    if(!tf) {
        e = E_TFILE_IO;
        err_msg(e);
        return e;
    }
    e = check_tilefile(tf);
    if(e) {
        err_msg(e);
        return e;
    }

    fclose(tf);
    return e;
}

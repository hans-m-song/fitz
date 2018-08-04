#include <stdio.h>
#include <stdlib.h>
#include "fitz.h"
#include "lib.h"

void end_game(Game *g) {
    ;
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

    end_game(&g);
    return e;
}

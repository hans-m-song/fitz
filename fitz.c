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
    if(g->tiles.data) {
        free(g->tiles.data);
    }
    if(g->board) {
        free(g->board);
    }
}

// loads save file data into game instance from given file
// returns an error code
int parse_sfile(Game *g, FILE *f) {
    char *str = malloc(sizeof(char) * MAX_BUFFER);
    str = fgets(str, MAX_BUFFER, f); // load game details
    if(!str) {
        return E_SFILE_R;
    }
    int output, numMoves, nextPlayer, row, col; // parse details
    output = sscanf(str, "%d %d %d %d", &numMoves, &nextPlayer, &row, &col);
    if(output != 4) {
        return E_SFILE_R;
    }
    g->numMoves = numMoves;
    g->nextPlayer = nextPlayer;
    g->dims[0] = row;
    g->dims[1] = col;
    #ifdef TEST
        fprintf(stdout, "got moves:%d player:%d row:%d col:%d\n", 
                numMoves, nextPlayer, row, col);
    #endif

    int i; // load saved board state
    for(i = 0; i < g->dims[0]; i++) {
        str = fgets(str, g->dims[1]+2, f);
        #ifdef TEST
            fprintf(stdout, "got str: %s\n", str);
        #endif
        if(!str || strlen(str) != g->dims[1]) {
            return E_SFILE_R;
        }
    }
    return OK;
}

// loads tiles into game instance from given file 
// @todo and prints them
int parse_tfile(Game *g, FILE *f) {
    g->tiles.depth = 1;
    g->tiles.data = malloc(sizeof(char) * TILE_MAX_ROW * TILE_MAX_COL);
    int i;
    char *str = malloc(sizeof(char) * TILE_MAX_ROW + 2);
    while(1) {
        for(i = 0; i < TILE_MAX_ROW + 1; i++) {
            if(!fgets(str, TILE_MAX_COL + 1, f)) { // get next line
                if (i < TILE_MAX_ROW) { // check if EOF is valid
                    return E_TFILE_R;
                } else {
                    // end condition
                    #ifdef TEST
                        fprintf(stdout, "got %d tiles\n", g->tiles.depth);
                    #endif 
                    return OK;
                }
            }
            #ifdef TEST
                fprintf(stdout, "got str: %s\n", str); 
            #endif
            char valid_chars[] = ",!";
            if(strlen(str) == TILE_MAX_ROW && 
                    strspn(str, valid_chars) == TILE_MAX_ROW) {
                fgetc(f); //consume trailing newline
                // save line
            } else if(i == 5 && strlen(str) == 1 && str[0] == '\n') {
                g->tiles.depth++; // increase memory of tiles for next tile
                g->tiles.data = realloc(g->tiles.data, sizeof(char) \
                        *  TILE_MAX_ROW * TILE_MAX_COL * g->tiles.depth);
                break;
            } else {
                return E_TFILE_R;
            }
        }
    }
    return OK;
}

// takes a file pointer to a file and game struct
// passes file to related function to check for correct formatting
// returns an error code
int check_file(Game *g, char type, char *filename) {
    FILE *f = fopen(filename, "r"); // check tilefile
    if(!f) {
        if(type == 't') {
            return E_TFILE_IO;
        } else {
            return E_SFILE_IO;
        }
    }
    #ifdef TEST
        fprintf(stdout, "got %c file %s\n", type, filename);
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
    char valid_chars[] = "12h";
    if(strlen(ptype) != 1 || strspn(ptype, valid_chars) != 1) {
        return E_PLAYER;
    }
    #ifdef TEST
        fprintf(stdout, "got player:%c\n", ptype[0]);
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
        fprintf(stdout, "got dimensions:%d,%d\n", g->dims[0], g->dims[1]);
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

    // end_game(&g);
    return e;
}

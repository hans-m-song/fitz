#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"
#include "lib.h"

// takes an integer representation of an error code
// prints the corresponding error code to stderr
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

// takes the game instance and a string containing the metadata
// parses the string and stores it in the game instance
// returns an error code
int parse_sfile_metadata(Game *g, char *str) {
    int output, numMoves, nextPlayer, row, col; // parse details
    output = sscanf(str, "%d %d %d %d", 
            &numMoves, &nextPlayer, &row, &col);
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
    
    return OK;
}
// loads save file data into game instance from given file
// returns an error code
int parse_sfile(Game *g, FILE *f) {
    char *meta_data = malloc(sizeof(char) * MAX_BUFFER);
    meta_data = fgets(meta_data, MAX_BUFFER, f); // load game details
    if(!meta_data) {
        free(meta_data);
        return E_SFILE_R;
    }

    if(parse_sfile_metadata(g, meta_data)) {
        free(meta_data);
        return E_SFILE_R;
    }
    free(meta_data);
    
    g->board = malloc(sizeof(char) * g->dims[0] * g->dims[1] + 1);
    int i; // load saved board state
    char *str = malloc(sizeof(char) * g->dims[1] + 1);
    for(i = 0; i < g->dims[0] + 1; i++) {
        str = fgets(str, g->dims[1] + 1, f);
        if((!str && i < g->dims[0]) || (str && i == g->dims[0])) { // check EOF
            free(str);
            return E_SFILE_R;
        } else if(!str && i == g->dims[0]) { // end case
            free(str);
            break;
        }
        
        char valid_chars[] = "*#.";
        if(strlen(str) != g->dims[1] || 
                strspn(str, valid_chars) != g->dims[1]) {
            free(str);
            return E_SFILE_R;
        }

        #ifdef TEST
            fprintf(stdout, "got str: %s\n", str);
        #endif
        
        fgetc(f); // consume trailing newline
        memcpy(g->board + (i * g->dims[1]), str, g->dims[1]);
    }
    g->board[g->dims[0] * g->dims[1]] = '\0';

    #ifdef TEST
        fprintf(stdout, "%s\n", g->board);
    #endif
    
    return OK;
}

// loads tiles into game instance from given file 
// @todo and prints them
int parse_tfile(Game *g, FILE *f) {
    g->numTiles = 1;
    g->tiles = malloc(sizeof(char*));
    
    while(1) {
        char str[TILE_MAX_COL + 1];
        g->tiles[g->numTiles - 1] = malloc(sizeof(char) * TILE_MAX_ROW * 
                TILE_MAX_COL + 1);
        int i;
        for(i = 0; i < TILE_MAX_ROW + 1; i++) {
            if(!fgets(str, TILE_MAX_COL + 1, f)) { // get line and check if EOF
                if (i < TILE_MAX_ROW - 1) { // check if EOF is valid
                    return E_TFILE_R;
                } else { // end case
                    int j;
                    for(j = 0; j < g->numTiles; j++) {
                        g->tiles[j][TILE_MAX_ROW * TILE_MAX_COL] = '\0';
                    }

                    #ifdef TEST
                        fprintf(stdout, "got %d tiles:\n", g->numTiles); 
                        int k;
                        for(k = 0; j < g->numTiles; k++) {
                            fprintf(stdout, "(%d)%s\n", k, g->tiles[k]);
                        }
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
                memcpy(g->tiles[g->numTiles-1] + (i * TILE_MAX_COL), 
                        str, TILE_MAX_ROW);
            } else if(i == TILE_MAX_ROW && strlen(str) == 1 &&
                    str[0] == '\n') { // check if to expect another tile
                g->numTiles++;
                g->tiles = realloc(g->tiles, sizeof(char*) * g->numTiles);
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
    FILE *f = fopen(filename, "r"); 
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


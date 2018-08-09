#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"
#include "init.h"
#include "tile.h"

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
    int output, nextTile, nextPlayer, row, col; // parse details
    output = sscanf(str, "%d %d %d %d", 
            &nextTile, &nextPlayer, &row, &col);
    if(output != 4 || nextTile > g->tileCount - 1 ||
            (nextPlayer != 0 && nextPlayer != 1)){
        return E_SFILE_R;
    }

    g->nextTile = nextTile;
    g->nextPlayer = nextPlayer;
    g->dims[0] = row;
    g->dims[1] = col;

    #ifdef TEST
        fprintf(stdout, "got moves:%d player:%d row:%d col:%d\n", 
                nextTile, nextPlayer, row, col);
    #endif
    
    return OK;
}
// loads save file data into game instance from given file
// returns an error code
int parse_sfile(Game *g, FILE *f) {
    char *meta_data = (char*)malloc(sizeof(char) * MAX_BUFFER);
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
    
    g->board = (char*)malloc(sizeof(char) * g->dims[0] * g->dims[1] + 1);
    int i; // load saved board state
    char *str = (char*)malloc(sizeof(char) * g->dims[1] + 1);
    for(i = 0; i < g->dims[0] + 1; i++) {
        str = fgets(str, g->dims[1] + 1, f);
        if(!str && i < g->dims[0]) { // check EOF
            free(g->board);
            free(str);
            return E_SFILE_R;
        } else if(i == g->dims[0] && str[0] == '\n') { // end case
            free(g->board);
            free(str);
            break;
        }
        
        char valid_chars[] = "*#.";
        if(strlen(str) != g->dims[1] || 
                strspn(str, valid_chars) != g->dims[1]) {
            free(g->board);
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

// retrieves 5 lines from the given file stream
// and saves it in the given array
// if a newline is aquired returns special value, else returns an error code
int get_tile(FILE *f, int tileSize, char output[tileSize + 1]) {
    char str[TILE_MAX_COL + 2];
    int i;
    for(i = 0; i < TILE_MAX_ROW; i++) {
        if(!fgets(str, TILE_MAX_COL + 2, f)) { // get line and check if EOF
            if(i == 0) { // check if EOF at beginning
                return E_EOF;
            }
            return E_TFILE_R;
        }
        
        if(strcmp(str, "\n") == 0 && i == 0) { // check if valid newline 
            output[0] = '\0';
            return UTIL;
        }

        char valid_chars[] = ",!\n";
        if(strlen(str) == TILE_MAX_ROW + 1 && 
                strspn(str, valid_chars) == TILE_MAX_ROW + 1) {
            str[TILE_MAX_COL] = '\0';
            memcpy(output + (i * TILE_MAX_COL), 
                    str, TILE_MAX_ROW);
        }

        #ifdef TEST
            fprintf(stdout, "got str: %s\n", str); 
        #endif
        
    }
    output[tileSize] = '\0';

    return OK;    
}

// loads tiles into game instance from given file 
// @todo and prints them
int parse_tfile(Game *g, FILE *f) {
    int tileSize = TILE_MAX_ROW * TILE_MAX_COL;
    g->tileCount = 0;
    g->tiles = (char**)malloc(sizeof(char*));
    g->tiles[g->tileCount] = (char*)malloc(sizeof(char) * tileSize + 1);
    char output[tileSize + 1]; // get_tile output stored here
    
    while(1) {
        int e = get_tile(f, tileSize, output);
        if(e == OK) {
            strcpy(g->tiles[g->tileCount], output);
            g->tileCount += 1;
            
            #ifdef TEST
                fprintf(stdout, "got tile:\n");
                print_tile(output);
            #endif 
        } else if (e == UTIL) { // expect another tile
            g->tiles = (char**)realloc(g->tiles, sizeof(char*) * 
                    (g->tileCount + 1));
            g->tiles[g->tileCount] = (char*)malloc(sizeof(char) * 
                    tileSize + 1);
        } else if(e == E_EOF) {
            return OK;
        } else {
            return e;
        }
    }
    
    #ifdef TEST
        fprintf(stdout, "got %d tiles\n", g->tileCount);
    #endif

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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"

//prints the given 1D representation of a tile in 2D
void print_tile(char *tiles) {
    int i, j;
    for(i = 0; i < TILE_MAX_ROW; i++) {
        for(j = 0; j < TILE_MAX_COL; j++) {
            fprintf(stdout, "%c", tiles[i * TILE_MAX_COL + j]);
        }
        fprintf(stdout, "\n");
    }

    #ifdef TEST
        fprintf(stdout, "\n");
    #endif

}

// uses the given tile and rotates it by the given degree (1:90, 2:180, 3:270)
// recurses until tile is rotated to degree
void rotate(int deg, int tileSize, char *tile, char output[tileSize + 1]) {
    if(!deg) {
        return; 
    }
    
    memset(output, '\0', tileSize + 1);
    //char *output = malloc(sizeof(char) * TILE_MAX_ROW * TILE_MAX_COL + 1);
    // for each row, rotate 90 degrees and put in corresponding column
    int i, j; // i:row, j:column where index is (i*row)+column
    for(i = 0; i < TILE_MAX_ROW; i++) {
        for(j = 0; j < TILE_MAX_COL; j++) {
            int in = i * TILE_MAX_ROW + j;
            int out = (j * TILE_MAX_COL) + TILE_MAX_ROW - i - 1;
            output[out] = tile[in];
        }
    }

    char temp[tileSize + 1];
    strcpy(temp, output);
    rotate(--deg, tileSize, output, temp);
    strcpy(output, temp);
}

// prints the tiles and their rotations using the given array of tiles
// saves a 2D array with each element being the tile and their 3 rotations
// as 1D strings
void print_all_tiles(char **tiles, int tileCount) {
    int tileSize = TILE_MAX_ROW * TILE_MAX_COL;
    char temp[tileSize + 1];
    memset(temp, '\0', tileSize + 1);
    return;
    char **output = (char**)malloc(sizeof(char*) * tileCount);
    int i;
    for(i = 0; i < tileCount; i++) {
        output[i] = (char*)malloc(sizeof(char) * tileSize * 4 + 1); 
        int k;
        for(k = 0; k < 4; k++) {
            memcpy(temp, tiles[i], tileSize);
            rotate(k, tileSize, tiles[i], temp);
            #ifdef TEST
                fprintf(stdout, "%d:\n", k * 90);
                print_tile(temp);
            #endif
            // memcpy(&output[i] + (k * tileSize), temp, tileSize + 1);
        }

    }

    int j;
    for(j = 0; j < tileCount; j++) {
        free(output[j]);
    }
    free(output);
}


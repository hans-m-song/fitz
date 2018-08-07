#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitz.h"

// uses the given tile and rotates it by the given degree (1:90, 2:180, 3:270)
// returns the rotated tile
char* rotate(char *tile) {
    char *output = malloc(sizeof(char) * TILE_MAX_ROW * TILE_MAX_COL + 1);
    // for each row, rotate 90 degrees and put in corresponding column
    int i, j; // i:row, j:column where index is (i*row)+column
    for(i = 0; i < TILE_MAX_ROW; i++) {
        for(j = 0; j < TILE_MAX_COL; j++) {
            int in = i * TILE_MAX_ROW + j;
            int out = (j * TILE_MAX_COL) + TILE_MAX_ROW - j - 1;
            output[out] = tile[in];
        }
    }
    output[TILE_MAX_ROW * TILE_MAX_COL] = '\0';
    return output;
}

// prints the tiles and their rotations using the given array of tiles
// saves a 2D array with each element being the tile and their 3 rotations
// as 1D strings
void print_all_tiles(char **tiles, int numTiles) {
    int tileSize = TILE_MAX_ROW * TILE_MAX_COL;
    
    char *temp;
    char **output = malloc(sizeof(char*) * numTiles);
    int i;
    for(i = 0; i < numTiles; i++) {
        output[i] = malloc(sizeof(char) * tileSize * 4 + 1);
        int k;
        for(k = 0; k < 3; k++) {
            temp = rotate(tiles[i]);
            #ifdef TEST
                fprintf(stdout, "%s:%d\n", temp, k * 90);
            #endif
            //memcpy(output[i] + (k * tileSize), temp, tileSize);
        }

    }
    free(temp);

    int j;
    for(j = 0; j < numTiles; j++) {
        free(output[j]);
    }
    free(output);
}

void print_tile(char *tiles) {
    int i, j;
    for(i = 0; i < TILE_MAX_ROW; i++) {
        for(j = 0; j < TILE_MAX_COL; j++) {
            fprintf(stdout, "%c", tiles[i * TILE_MAX_COL + j]);
        }
        fprintf(stdout, "\n");
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tile.h"

// frees memory used for the given number of tiles in the given tile array 
void clear_tiles(char** tiles, int tileCount) {
    int i;
    for(i = 0; i < tileCount; i++) {
        free(tiles[i]);
    }
    free(tiles);
}

// prints the given 1D representation of a tile in 2D
// where tile[row][col] -> tile[(row * width) + col]
void print_tile(char* tile) {
    int i, j;
    for(i = 0; i < TILE_MAX_ROW; i++) {
        for(j = 0; j < TILE_MAX_COL; j++) {
            fprintf(stdout, "%c", tile[i * TILE_MAX_COL + j]);
        }
        fprintf(stdout, "\n");
    }
    fflush(stdout);
}

// takes a string representing a tile and it's 3 valid rotations and prints
// them side by side
void print_side_by_side(char* tiles) {
    int i, j, k;
    for(i = 0; i < TILE_MAX_ROW; i++) { // for each row
        for(j = 0; j < 4; j++) { // for each rotation
            for(k = 0; k < TILE_MAX_COL; k++) { // for each element
                int index = (j * TILE_SIZE) + (i * TILE_MAX_COL) + k;
                fprintf(stdout, "%c", tiles[index]);
            }
            if(j < 3) { // ommit space on last tile
                fprintf(stdout, " ");
            }
        }
        fprintf(stdout, "\n");
    }
    fflush(stdout);
}

// rotates the given tile once and saves it to the given string 
// recurses until tile is rotated by the given degree (1:90, 2:180, 3:270)
void rotate(int deg, char* tile, char output[TILE_SIZE + 1]) {
    if(!deg) {
        memcpy(output, tile, TILE_SIZE + 1);
        return; 
    }
    
    memset(output, '\0', TILE_SIZE + 1);
    // for each row, rotate 90 degrees and put in corresponding column
    // i:row, j:col where index of source (row_max * row) + col is 
    // mapped to (col * col_max) + (row_max - row - 1)
    int i, j; 
    for(i = 0; i < TILE_MAX_ROW; i++) {
        for(j = 0; j < TILE_MAX_COL; j++) {
            int in = i * TILE_MAX_ROW + j;
            int out = (j * TILE_MAX_COL) + TILE_MAX_ROW - i - 1;
            output[out] = tile[in];
        }
    }

    char temp[TILE_SIZE + 1];
    strcpy(temp, output);
    rotate(--deg, output, temp);
    strcpy(output, temp);
}

// calls rotate on the given array of tiles and stores them for printing
// saves a 2D array with each element being the tile and their 3 rotations
// as 1D strings
void print_all_tiles(char** tiles, int tileCount) {
    char** output = (char**)malloc(sizeof(char*) * tileCount);
    char temp[TILE_SIZE + 1]; // buffer to save rotated tile to
    memset(temp, '\0', TILE_SIZE + 1);

    int i;
    for(i = 0; i < tileCount; i++) { // retrieve rotated versions of tiles
        output[i] = (char*)malloc(sizeof(char) * TILE_SIZE * 4 + 1); 
        int k;
        for(k = 0; k < 4; k++) { // rotate each tile by the 3 angles
            memcpy(temp, tiles[i], TILE_SIZE);
            rotate(k, tiles[i], temp);
            
#ifdef VERBOSE 
            fprintf(stdout, "%d:\n", k * 90);
            print_tile(temp);
#endif
            
            memcpy(output[i] + (k * TILE_SIZE), temp, TILE_SIZE);
        }
        output[i][TILE_SIZE * 4] = '\0';
    }

    int l;
    for(l = 0; l < tileCount; l++) {
        print_side_by_side(output[l]);
        if(l < tileCount - 1) { // ommit newline seperator on last tile
            fprintf(stdout, "\n");
        }
    }

    int j; // deallocates memory used
    for(j = 0; j < tileCount; j++) {
        free(output[j]);
    }
    free(output);
}


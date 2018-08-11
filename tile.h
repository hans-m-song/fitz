#ifndef TILE_H
#define TILE_H

#define TILE_MAX_ROW 5
#define TILE_MAX_COL 5
#define TILE_SIZE 25

void clear_tiles(char** tiles, int tileCount);

void print_tile(char* tiles);

void rotate(int deg, char* tile, char output[]);

void print_all_tiles(char** tiles, int numTiles);

#endif

#ifndef PLAYER_H
#define PLAYER_H

#include "fitz.h"

int valid_move(Game* g, int r, int c, int theta);

int a1_move(Game* g, Move* m);

int a2_move(Game* g, Move* m);

int h_move(Game* g, Move* m);

#endif

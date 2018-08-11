#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include "fitz.h"

void err_msg(Err e);

int parse_sfile(Game* g, FILE* f);

int parse_tfile(Game* g, FILE* f);

int check_file(Game* g, char type, char* filename);

int check_player(char* ptype);

int check_dims(Game* g, char* row, char* col);

void init_move(Move m, char ptype);

#endif

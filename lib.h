#ifndef LIB_H
#define LIB_H

void err_msg(Err e);

int parse_sfile(Game *g, FILE *f);

int parse_tfile(Game *g, FILE *f);

int check_file(Game *g, char type, char *filename);

int check_player(char *ptype);

int check_dims(Game *g, char *row, char *col);

#endif

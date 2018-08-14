CC = gcc
BUILD := release
flags.test := -DTEST -std=c99 -Wall -pedantic -g
flags.verbose := -DTEST -DVERBOSE -std=c99 -Wall -pedantic -g
flags.release := -std=c99 -Wall -pedantic 
FLAGS := ${flags.${BUILD}} 
OBJ = init.o fitz.o tile.o player.o

all: fitz
	@echo BUILD=${BUILD}

fitz: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o fitz 

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<

.PHONY: all

clean:
	rm -f fitz *.o

CC = gcc
BUILD := test
flags.test := -DTEST -std=c99 -Wall -pedantic -g
flags.release := -std=c99 -Wall -pedantic 
FLAGS := ${flags.${BUILD}} 
OBJ = lib.o fitz.o

all: fitz
	@echo BUILD=${BUILD}

fitz: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o fitz 

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<

debugd:
	valgrind --leak-check=full --track-origins=yes ./fitz tf 1 1 5 5

debugs:
	valgrind --leak-check=full --track-origins=yes ./fitz tf 1 1 sav

.PHONY: all

clean:
	rm -f fitz *.o

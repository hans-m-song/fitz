CC = gcc
BUILD := test
flags.test := -DTEST -std=c99 -Wall -pedantic -g
flags.verbose := -DTEST -DVERBOSE -std=c99 -Wall -pedantic -g
flags.release := -std=c99 -Wall -pedantic 
FLAGS := ${flags.${BUILD}} 
OBJ = init.o fitz.o tile.o

all: fitz
	@echo BUILD=${BUILD}

fitz: $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o fitz 

%.o: %.c
	$(CC) $(FLAGS) -c -o $@ $<

dim:
	valgrind --leak-check=full --track-origins=yes ./fitz tf 1 1 5 5

sav:
	valgrind --leak-check=full --track-origins=yes ./fitz tf 1 1 sav

badsav:
	valgrind --leak-check=full --track-origins=yes ./fitz tf 1 1 badsav

tf:
	valgrind --leak-check=full --track-origins=yes ./fitz tf

.PHONY: all

clean:
	rm -f fitz *.o

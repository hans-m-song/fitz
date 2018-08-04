CC = gcc
FLAGS = -std=c99 -Wall -pedantic
OBJ = lib.o

all: fitz

fitz: $(OBJ) fitz.o
	$(CC) $(FLAGS) $(OBJ) fitz.c -o fitz 

test: all -DTEST

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm -f fitz *.o

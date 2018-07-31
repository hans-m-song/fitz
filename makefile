CC=gcc
FLAGS=-std=c99 -Wall -pedantic

.PHONY: clean all

all: fitz.h
	$(CC) fitz.c -o fitz $(FLAGS)

clean:
	rm fitz

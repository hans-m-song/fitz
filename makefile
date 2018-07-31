CC=gcc
FLAGS=-std=c99

.PHONY: clean all

all: fitz.h
	$(CC) fitz.c -o fitz $(FLAGS)

clean:
	rm fitz

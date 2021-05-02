CC := gcc
CFLAGS := --std=c11 -Wall -fopenmp
OBJ := obj
SRC := src
LIB := lib

.PHONY: all clean zip

all: main

main: $(OBJ)/main.o $(OBJ)/Lab3IO.o
	$(CC) $(CFLAGS) $(OBJ)/main.o $(OBJ)/Lab3IO.o -o main

$(OBJ)/main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c $(SRC)/main.c -o $(OBJ)/main.o

$(OBJ)/Lab3IO.o: $(LIB)/Lab3IO.c
	$(CC) $(CFLAGS) -c $(LIB)/Lab3IO.c -o $(OBJ)/Lab3IO.o

clean:
	@rm -rf $(OBJ)/* main

zip:
	make clean
	cd .. && zip -r 1350542 Code Report Members && cd -

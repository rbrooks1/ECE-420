CC := mpicc
CFLAGS := --std=c11 -Wall
LDFLAGS := -lm
OBJ := obj
SRC := src
LIB := lib

.PHONY: all clean zip

all: main

main: $(OBJ)/main.o $(OBJ)/Lab4_IO.o
	$(CC) $(CFLAGS) $(OBJ)/main.o $(OBJ)/Lab4_IO.o $(LDFLAGS) -o main

$(OBJ)/main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c $(SRC)/main.c -o $(OBJ)/main.o

$(OBJ)/Lab4_IO.o: $(LIB)/Lab4_IO.c
	$(CC) $(CFLAGS) -c $(LIB)/Lab4_IO.c -o $(OBJ)/Lab4_IO.o

clean:
	@rm -rf $(OBJ)/* main

zip:
	make clean
	cd .. && zip -r 1350542 Code Report Members && cd -

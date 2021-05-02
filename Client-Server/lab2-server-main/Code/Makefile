CC := gcc
CFLAGS := --std=gnu99 -Wall
LDFLAGS := -lpthread
OBJ := obj
SRC := src
LIB := lib

.PHONY: all clean zip

all: main1 main2 main3 main4

main1: $(OBJ)/main1.o
	$(CC) $(CFLAGS) $(OBJ)/main1.o $(LDFLAGS) -o main1

main2: $(OBJ)/main2.o
	$(CC) $(CFLAGS) $(OBJ)/main2.o $(LDFLAGS) -o main2

main3: $(OBJ)/main3.o $(OBJ)/mylib_rwlock_t.o
	$(CC) $(CFLAGS) $(OBJ)/main3.o $(OBJ)/mylib_rwlock_t.o $(LDFLAGS) -o main3

main4: $(OBJ)/main4.o $(OBJ)/mylib_rwlock_t.o
	$(CC) $(CFLAGS) $(OBJ)/main4.o $(OBJ)/mylib_rwlock_t.o $(LDFLAGS) -o main4

$(OBJ)/main1.o: $(SRC)/main1.c
	$(CC) $(CFLAGS) -c $(SRC)/main1.c -o $(OBJ)/main1.o

$(OBJ)/main2.o: $(SRC)/main2.c
	$(CC) $(CFLAGS) -c $(SRC)/main2.c -o $(OBJ)/main2.o

$(OBJ)/main3.o: $(SRC)/main3.c
	$(CC) $(CFLAGS) -c $(SRC)/main3.c -o $(OBJ)/main3.o

$(OBJ)/main4.o: $(SRC)/main4.c
	$(CC) $(CFLAGS) -c $(SRC)/main4.c -o $(OBJ)/main4.o

$(OBJ)/mylib_rwlock_t.o: $(SRC)/mylib_rwlock_t.c $(SRC)/mylib_rwlock_t.h
	$(CC) $(CFLAGS) -c $(SRC)/mylib_rwlock_t.c -o $(OBJ)/mylib_rwlock_t.o

clean:
	@rm -rf $(OBJ)/* main1 main2 main3 main4 server_output_time_aggregated

zip:
	make clean
	@zip -r ../1350542 ../Code ../Report ../Members
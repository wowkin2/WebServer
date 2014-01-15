# Compilation
CFLAGS= -o
CC=gcc $(CFLAGS)
# Headers
HEAD=server.h

# binary folder
BIN=bin/
# source folder
SRC=src/


all: main

main:
    $(CC) $(BIN)main $(SRC)main.c

rebuild: clean all

clean:
    rm -rf *o main

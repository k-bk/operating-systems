CC = gcc
CFLAGS = -Wall
DEPS = 
OBJ = main.o

.PHONY: all clean program bash

all: program bash

main_bash.o: main.c 
	$(CC) -c -o main_bash.o $< $(CFLAGS) -DBASH

bash: main_bash.o 
	$(CC) -o program_bash main_bash.o $(CFLAGS) -DBASH

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

program: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

clean: 
	rm -rf *.o *.so *.a program program_bash

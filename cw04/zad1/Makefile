CC = gcc
CFLAGS = -Wall -std=c99 -I.
DEPS = 
OBJ = main.o

all: program

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

program: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm -rf *.o *.so *.a program 

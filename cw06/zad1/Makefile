CC = gcc
CFLAGS = -Wall
DEPS = 
OBJ = main.o

.PHONY: all clean program

all: program

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

program: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

clean: 
	rm -rf *.o *.so *.a program

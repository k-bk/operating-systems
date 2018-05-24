CC = gcc
CFLAGS = -Wall

all: filter

filter: filter.c
	$(CC) filter.c -o filter $(CFLAGS)

tests: filter
	./filter 5 brain.ascii.pgm mock out.pgm
clean:
	rm -rf filter *.o

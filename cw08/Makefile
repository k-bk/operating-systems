CC = gcc
CFLAGS = -Wall 
LIBS = -lpthread

all: filter

filter: filter.c
	$(CC) filter.c -o filter $(CFLAGS) $(LIBS)

tests: filter
	./filter 1 mountain.ascii.pgm fil1 out1.pgm
	./filter 2 mountain.ascii.pgm fil1 out1.pgm
	./filter 4 mountain.ascii.pgm fil1 out1.pgm
	./filter 8 mountain.ascii.pgm fil1 out1.pgm

	./filter 1 mountain.ascii.pgm fil2 out2.pgm
	./filter 2 mountain.ascii.pgm fil2 out2.pgm
	./filter 4 mountain.ascii.pgm fil2 out2.pgm
	./filter 8 mountain.ascii.pgm fil2 out2.pgm
clean:
	rm -rf filter *.o

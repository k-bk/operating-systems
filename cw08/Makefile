CC = gcc
CFLAGS = -Wall 
LIBS = -lpthread

all: filter

filter: filter.c
	$(CC) filter.c -o filter $(CFLAGS) $(LIBS)

tests: filter
	./filter 5 mountain.ascii.pgm fil1 out1.pgm
	./filter 5 mountain.ascii.pgm fil2 out2.pgm
	./filter 5 mountain.ascii.pgm fil3 out3.pgm
clean:
	rm -rf filter *.o

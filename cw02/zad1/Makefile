CC = gcc
CFLAGS = -Wall -std=c99 -ggdb

test: main.o lib_iocmp.o
	$(CC) $(CFLAGS) -o test

main.o: main.c lib_iocmp.h
	$(CC) $(CFLAGS) -c main.c

iocmp.o: lib_iocmp.c lib_iocmp.h
	$(CC) $(CFLAGS) -c lib_iocmp.c

clean: 
	rm -f *.o *.so *.a test

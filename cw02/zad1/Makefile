CC = gcc
CFLAGS = -Wall -std=c99 -ggdb

test: main.o iocmp.o
	$(CC) $(CFLAGS) -o test

main.o: main.c iocmp.h
	$(CC) $(CFLAGS) -c main.c

iocmp.o: iocmp.c sys_iocmp.c iocmp.h
	$(CC) $(CFLAGS) -c iocmp.c

clean: 
	rm -f *.o *.so *.a test

CFLAGS = -Wall -std=c99 -ggdb

test: main.o iocmp.o
	gcc $(CFLAGS) -o test main.o iocmp.o

main.o: main.c iocmp.h
	gcc $(CFLAGS) -c main.c

iocmp.o: iocmp.c iocmp.h
	gcc $(CFLAGS) -c iocmp.c

clean: 
	rm -f *.o *.so *.a test

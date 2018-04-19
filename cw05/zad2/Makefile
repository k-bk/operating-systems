CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all: master slave

master: master.o 
	$(CC) $(CFLAGS) -o master master.o

slave: slave.o 
	$(CC) $(CFLAGS) -o slave slave.o

master.o: master.c
	$(CC) $(CFLAGS) -c master.c

slave.o: slave.c
	$(CC) $(CFLAGS) -c slave.c

clean: 
	rm -f *.o program master slave

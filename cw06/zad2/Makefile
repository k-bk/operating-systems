CC = gcc
CFLAGS = -Wall
DEPS = 
OBJ = server.o client.o

.PHONY: all clean

all: server client 

server.o: server.c protocol.h
	$(CC) -c server.c $(CFLAGS)

client.o: client.c protocol.h
	$(CC) -c client.c $(CFLAGS)

clean: 
	rm -rf *.o *.so *.a server client

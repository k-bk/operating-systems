CC = gcc
CFLAGS = -Wall -lrt
DEPS = protocol.h
OBJ = server.o client.o

.PHONY: all clean

all: server client 

server: server.o
	$(CC) -o server $< $(CFLAGS)

client: client.o
	$(CC) -o client $< $(CFLAGS)

server.o: server.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

client.o: client.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

clean: 
	rm -rf *.o *.so *.a server client

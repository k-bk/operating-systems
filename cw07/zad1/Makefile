CC = gcc
CFLAGS = -Wall
LIBS = messages.o

all: clean barber clients

barber:  
	$(CC) barber.c messages.c -o barber $(CFLAGS) 

clients:  
	$(CC) clients.c messages.c -o clients $(CFLAGS) 

clean: 
	rm -rf *.o barber clients

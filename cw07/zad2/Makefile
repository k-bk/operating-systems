CC = gcc
CFLAGS = -Wall -lrt
LIBS = -lpthread

all: clean barber clients

barber:  
	$(CC) barber.c messages.c $(LIBS) -o barber $(CFLAGS) 

clients:  
	$(CC) clients.c messages.c $(LIBS) -o clients $(CFLAGS) 

clean: 
	rm -rf *.o barber clients

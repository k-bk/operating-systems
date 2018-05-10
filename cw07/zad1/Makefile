CC = gcc
CFLAGS = -Wall
LIBS = messages.l

all: barber

%.o: %.c
	$(CC) -c $^ $(CFLAGS)

clean: 
	rm -rf *.o 

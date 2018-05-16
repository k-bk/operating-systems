CC = gcc
CFLAGS = -Wall
LIBS = messages.o

all: barber

barber: $<.c $(LIBS)
	$(CC) -o $< $^ $(CFLAGS)

%.o: %.c
	$(CC) -c $^ $(CFLAGS)

clean: 
	rm -rf *.o 

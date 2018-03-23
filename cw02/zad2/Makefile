CC = gcc
CFLAGS = -Wall -std=gnu99 -I.
DEPS = 
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm -f *.o *.so *.a test

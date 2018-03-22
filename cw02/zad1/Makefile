CC = gcc
CFLAGS = -Wall -std=c99 -ggdb -I.
DEPS = lib_iocmp.h sys_iocmp.h
OBJ = sys_iocmp.o lib_iocmp.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm -f *.o *.so *.a test

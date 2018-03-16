CFLAGS = -Wall -std=c99 -ggdb

all: static shared clean

static:
	gcc $(CFLAGS) -c array_manager.c
	ar rcs array_manager.a array_manager.o 
shared:
	gcc $(CFLAGS) -c -fPIC array_manager.c
	gcc $(CFLAGS) -shared -fPIC -o array_manager.so array_manager.o

clean: 
	rm -f *.o *.so *.a

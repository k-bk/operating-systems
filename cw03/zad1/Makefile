CC = gcc
CFLAGS = -Wall -std=gnu99 -I.
DEPS = 
OBJ = main.o

all: program testdirs

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

program: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

testdirs:
	mkdir -p dir1/dir2
	echo "asfdjaksjl" > textFile
	echo "asgfkdsljd" > dir1/text2
	echo "lgrepjpjjo" > dir1/dir2/text3
	ln -s dir1/dir2/text3/ dir1/link1

clean: 
	rm -rf *.o *.so *.a program dir1 textFile

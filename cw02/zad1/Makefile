CC = gcc
CFLAGS = -Wall -std=c99 -ggdb -I.
OUT = result.txt
DEPS = lib_iocmp.h sys_iocmp.h
OBJ = sys_iocmp.o lib_iocmp.o main.o
NUM = 100000
BUFF = 512

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean: 
	rm -f *.o *.so *.a test file1 file2

compare:
	./test generate file1 $(NUM) $(BUFF) sys >> $(OUT)
	cp file1 file2
	./test sort file1 $(NUM) $(BUFF) sys >> $(OUT)
	./test sort file1 $(NUM) $(BUFF) lib >> $(OUT)
	./test copy file1 file2 $(NUM) $(BUFF) sys >> $(OUT)
	./test copy file1 file3 $(NUM) $(BUFF) lib >> $(OUT)

CC = gcc
CFLAGS = -Wall
DEPS = 
OBJ = main.o
DEF =  

.PHONY: all clean program

all: program

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(addprefix -D, $(DEF))

program: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(addprefix -D, $(DEF))

clean: 
	rm -rf *.o *.so *.a program 

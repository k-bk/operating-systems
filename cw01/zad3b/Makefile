OUT = results3b3.txt 
CFLAGS = -Wall -std=c99 -O3

all: static shared clean

static:
	gcc $(CFLAGS) -c array_manager.c
	ar rcs array_manager.a array_manager.o 
shared:
	gcc $(CFLAGS) -c -fPIC array_manager.c
	gcc $(CFLAGS) -shared -fPIC -o array_manager.so array_manager.o

zad3_static:
	make static
	gcc $(CFLAGS) -o manager main.c array_manager.a -I .
	make tests

zad3_shared:
	make shared
	gcc $(CFLAGS) -o manager main.c -L. array_manager.so -I .
	make tests

zad3_dynamic:
	make shared
	gcc $(CFLAGS) -o manager main.c -D DYNAMIC 

clean: 
	rm -f *.o *.a *.so

clean_report:
	rm -f *.txt

ST_TEST = ./manager $(OUT) 1 create_table 
DY_TEST = ./manager $(OUT) 0 create_table 

tests:
	# using static allocation
	$(ST_TEST) 20000 100
	$(ST_TEST) 20000 100 remove 10000 add 10000
	$(ST_TEST) 20000 100 remove_and_add 10000
	
	# using dynamic allocation
	$(DY_TEST) 20000 100 
	$(DY_TEST) 20000 100 remove 10000 add 10000
	$(DY_TEST) 20000 100 remove_and_add 10000
	

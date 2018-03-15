CFLAGS = -Wall -std=c99

all: static shared clean

static:
	gcc $(CFLAGS) -c array_manager.c
	ar rcs array_manager.a array_manager.o 
shared:
	gcc $(CFLAGS) -c -fPIC array_manager.c
	gcc $(CFLAGS) -shared -fPIC -o array_manager.so array_manager.o

zad3_static:
	make static
	gcc $(CFLAGS) -o manager main.c array_manager.a -I . -ggdb -O0
	make tests
	
zad3_dynamic:
	make shared
	gcc $(CFLAGS) -o manager main.c -I . -Wall -ggdb -O0
	make tests

zad3_shared:
	make shared
	gcc -o manager main.c -L. array_manager.so -std=c99 -I . -Wall -ggdb -O0
	make tests

clean: 
	rm -f *.o

ST_TEST = ./manager r_static.txt create_table 
DY_TEST = ./manager r_dynamic.txt create_table 

tests:
	# using static allocation
	$(ST_TEST) 2000 100 static
	$(ST_TEST) 2000 100 static search_element 100
	$(ST_TEST) 2000 100 static remove 1000 add 1000
	$(ST_TEST) 2000 100 static remove_and_add 1000
	
	# using dynamic allocation
	$(DY_TEST) 2000 100 static
	$(DY_TEST) 2000 100 static search_element 100
	$(DY_TEST) 2000 100 static remove 1000 add 1000
	$(DY_TEST) 2000 100 static remove_and_add 1000
	
	# using dynamic allocation
	./manager r_dynamic.txt 2000 100 dynamic
	./manager r_dynamic.txt 2000 100 dynamic search_element 100
	./manager r_dynamic.txt 2000 100 dynamic remove 1000 add 1000
	./manager r_dynamic.txt 2000 100 dynamic remove_and_add 1000

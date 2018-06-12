CC = gcc
CFLAGS = -Wall 
DEPS = -lpthread
LIBS = arrays.o cfgparser.o

all: jobs

jobs: jobs.c $(LIBS) 
	$(CC) -o jobs jobs.c $(LIBS) $(CFLAGS) $(DEPS)
arrays.o: ../arrays.c
	$(CC) -c ../arrays.c $(CFLAGS)
cfgparser.o: ../cfgparser.c
	$(CC) -c ../cfgparser.c $(CFLAGS)


tests: jobs
clean:
	rm -rf jobs *.o

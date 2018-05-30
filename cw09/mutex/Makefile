CC = gcc
CFLAGS = -Wall 
DEPS = -lpthread
LIBS = arrays cfgparser

all: jobs

jobs: jobs.c $(LIBS) 
	$(CC) jobs.c -l $(LIBS) -L ./ -o jobs $(CFLAGS) $(DEPS)
arrays.o: arrays.c
	$(CC) arrays.c $(CFLAGS)
	ar rcs arrays.a arrays.o
cfgparser.o: cfgparser.c
	$(CC) cfgparser.c $(CFLAGS)
	ar rcs cfgparser.a cfgparser.o


tests: jobs
clean:
	rm -rf jobs *.o

CC = gcc
CFLAGS = -Wall 
LIBS = -lpthread

all: jobs

jobs: jobs.c
	$(CC) jobs.c -o jobs $(CFLAGS) $(LIBS)

tests: jobs
clean:
	rm -rf jobs *.o

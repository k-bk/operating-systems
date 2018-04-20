#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>

void free_resources () {
    printf ("ending everything\n");
}

int main () {
    char* pathName = "main.c";
    char proj = 'a';
    key_t key = ftok (pathName, proj);
    printf("key: %d\n", key);
    atexit (free_resources);
    exit (EXIT_SUCCESS);
}

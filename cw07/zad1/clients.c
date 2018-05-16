#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include "shared_barber.h"
#include "messages.h"

// ---------- Semaphores ---------------------------
shared *state;

// ---------- Communication ------------------------

void err (const char *msg) {
    errno != 0 ? perror(msg) : printf(COLOR_RED"%s\n", msg);
    exit(EXIT_FAILURE);
}

void client (int haircuts_needed) {
    id_msg my_id;
    my_id.pid = getpid();
    int haircuts = 0;

    while (haircuts < haircuts_needed) {
        sem_take(state->change_waiting_room);
        log_message("I am entering the waiting room");
        //sem_take(state->
    }
}

// ---------- Main program -------------------------

int main (int argc, char **argv) {

    if (argc < 3) {
        printf(COLOR_RED "%s: not enough arguments\n" COLOR_RESET
               "Usage: '%s <num_of_clients> <num_of_haircuts>'\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    int num_of_clients = atoi(argv[1]);
    int haircuts_needed = atoi(argv[2]);

    char *home_path = getenv("HOME");
    if (home_path == NULL) err("getenv");

    int shmid = 
        shmget(ftok(home_path, 0), sizeof(shared), 0);
    state = shmat(shmid, NULL, 0);


    while(1) {
        sem_take(state->change_waiting_room);
        sem_take(state->barber_ready);
        printf("i take your readiness\n");
        sem_give(state->customers_ready);
        sem_give(state->change_waiting_room);
    }

//    client(haircuts_needed);

    return EXIT_SUCCESS;
}

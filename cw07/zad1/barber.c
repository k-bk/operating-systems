#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include "shared_barber.h"
#include "messages.h"

// ---------- Semaphores ---------------------------
shared *state;

// ---------- Communication ------------------------

void err (const char *msg) {
    errno != 0 ? perror(msg) : printf(COLOR_RED"%s\n", msg);
    exit(EXIT_FAILURE);
}

void barber () {
    id_msg first_client;
    while (1) {
        log_message("I am falling asleep... zzz...");
        sem_take(state->customers_ready);
        log_message("Woken up!");
        sem_take(state->change_waiting_room);
        // take first pid from FIFO
        msgrcv(state->waiting_room, 
                &first_client, 
                sizeof(id_msg),
                /*msgtyp*/ 0, 
                /*msgflg*/ 0);
        log_message("Please %d, come here->", first_client.pid);
        sem_give(state->barber_ready);
        log_message("Mr %d, I am ready for the haircut.", first_client.pid);
        sem_give(state->change_waiting_room);
        log_message("Mr %d, I have finished your haircut.", first_client.pid);
    }
}

// ---------- Main program -------------------------

int main (int argc, char **argv) {

    log_message("Hello my friend");

    if (argc < 2) {
        printf(COLOR_RED "%s: not enough arguments\n" COLOR_RESET
               "Usage: '%s <num_of_chairs>'\n", argv[0], argv[0]);
    }
    char *home_path = getenv("HOME");
    if (home_path == NULL) err("getenv");

    int shmid = 
        shmget(ftok(home_path, 0), sizeof(shared), IPC_CREAT | S_IWUSR | S_IRUSR);
    state = shmat(shmid, NULL, 0);

    state->barber_ready = 
        semget(ftok(home_path, 1), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    state->customers_ready = 
        semget(ftok(home_path, 2), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    state->change_waiting_room = 
        semget(ftok(home_path, 3), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    state->waiting_room =
        msgget(ftok(home_path, 4), IPC_CREAT | S_IWUSR | S_IRUSR);

    barber();

    return EXIT_SUCCESS;
}

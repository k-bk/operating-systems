#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include "shared_barber.h"
#include "messages.h"

// ---------- Semaphores ---------------------------
shared *state;

// ---------- Utilities ----------------------------

void err (const char *msg) {
    errno != 0 ? perror(msg) : printf(COLOR_RED"%s\n", msg);
    exit(EXIT_FAILURE);
}

void use_sigint (int _) {
    exit(EXIT_FAILURE);
}

void clean_up () {
    semctl(state->barber_ready, 0, IPC_RMID);
    semctl(state->customers_ready, 0, IPC_RMID);
    semctl(state->change_waiting_room, 1, IPC_RMID);
    semctl(state->chair, 0, IPC_RMID);
    msgctl(state->waiting_room, IPC_RMID, NULL);
}

// ---------- Semaphore loop -----------------------

void barber () {
    id_msg first_client;
    while (1) {
        log_message("I am falling asleep... zzz...");
        sem_take(state->customers_ready);
        log_message("Woken up!");
        sem_take(state->change_waiting_room);
        // take first pid from FIFO
        msgrcv(state->waiting_room, &first_client, sizeof(id_msg), 0, 0);
        log_message("Please %d, come here->", first_client.pid);
        sem_give(state->barber_ready);
        log_message("Mr %d, I can give you a haircut.", first_client.pid);
        sem_give(state->change_waiting_room);
        sem_take(state->chair);
        log_message("Mr %d, I am doing your haircut.", first_client.pid);
        sem_give(state->chair);
        log_message("Mr %d, I have finished your haircut.", first_client.pid);
    }
}

// ---------- Main program -------------------------

int main (int argc, char **argv) {
    signal(SIGINT, use_sigint);
    if (argc < 2) {
        printf(COLOR_RED "%s: not enough arguments\n" COLOR_RESET
               "Usage: '%s <num_of_chairs>'\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }
    char *home_path = getenv("HOME");
    if (home_path == NULL) err("getenv");

    int shmid = shmget(ftok(home_path, 0), sizeof(shared), IPC_CREAT | 0666);
    printf("size: %lu\n", sizeof(shared));
    state = shmat(shmid, NULL, 0);

    state->barber_ready = sem_make_and_set(0);
    state->customers_ready = sem_make_and_set(0);
    state->change_waiting_room = sem_make_and_set(1);
    state->chair = sem_make_and_set(1);
    state->waiting_room = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    atexit(clean_up);

    barber();

    return EXIT_SUCCESS;
}

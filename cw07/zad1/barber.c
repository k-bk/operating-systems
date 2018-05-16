#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stat.h>
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

void clean_up () {
    semctl(state->barber_ready, 0, IPC_RMID);
    semctl(state->customers_ready, 0, IPC_RMID);
    semctl(state->change_waiting_room, 1, IPC_RMID);
    semctl(state->chair, 0, IPC_RMID);
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
    semctl(state->barber_ready, 0, SETVAL);

    state->customers_ready = 
        semget(ftok(home_path, 2), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    semctl(state->customers_ready, 0, SETVAL);

    state->change_waiting_room = 
        semget(ftok(home_path, 3), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    semctl(state->change_waiting_room, 1, SETVAL);

    state->chair = 
        semget(ftok(home_path, 4), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    semctl(state->chair, 1, SETVAL);

    state->waiting_room =
        msgget(ftok(home_path, 5), IPC_CREAT | S_IWUSR | S_IRUSR);

    atexit(clean_up);

    printf("Semaphores set.\n");

    while(1) {
        sem_take(state->change_waiting_room);
        sem_give(state->barber_ready);
        printf("i am ready\n");
        sem_give(state->change_waiting_room);
        printf("waiting for customer\n");
        sem_take(state->customers_ready);
        printf("you arrived\n");
    }

    //barber();

    return EXIT_SUCCESS;
}

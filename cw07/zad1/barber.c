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
int shmid = 0;
shared *state;

// ---------- Utilities ----------------------------

void err (const char *msg) {
    errno != 0 ? perror(msg) : printf(C_RED "%s\n", msg);
    exit(EXIT_FAILURE);
}

void use_sigint (int _) {
    exit(EXIT_SUCCESS);
}

void clean_up () {
    semctl(state->customers_waiting, 0, IPC_RMID);
    semctl(state->change_WR, 0, IPC_RMID);
    semctl(state->WR_places, 0, IPC_RMID);
    semctl(state->chair, 0, IPC_RMID);
    msgctl(state->waiting_room, IPC_RMID, NULL);
    if (shmdt(state) == -1) perror("shmdt");
    shmctl(shmid, IPC_RMID, NULL);
}

// ---------- Semaphore loop -----------------------

void barber () {
    id_msg first_client;
    while (1) {
        sem_take(state->change_WR);
        if (sem_getval(state->customers_waiting) == 0) {
            if (state->barber != ASLEEP) 
                log_message("I am falling asleep... zzz...");
            state->barber = ASLEEP;
            sem_give(state->change_WR);
        } else {
            sem_take(state->customers_waiting);
            state->barber = WORKING;
            log_message("Woken up!");
            msgrcv(state->waiting_room, &first_client, sizeof(id_msg), 0, 0);
            sem_give(state->change_WR);
            kill(first_client.pid, SIGUSR1);
            log_message("%d\t Please, come here.", first_client.pid);
            sem_take(state->change_WR);
            sem_give(state->chair);
            log_message("%d\t I am doing your haircut.", first_client.pid);
            log_message("%d\t I have finished your haircut.", first_client.pid);
            sem_give(state->change_WR);
        }
    }
}

// ---------- Main program -------------------------

int main (int argc, char **argv) {
    signal(SIGINT, use_sigint);
    if (argc < 2) {
        printf(C_RED "%s: not enough arguments\n" C_RESET
               "Usage: '%s <num_of_chairs>'\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
    }
    char *home_path = getenv("HOME");
    if (home_path == NULL) err("getenv");

    shmid = shmget(ftok(home_path, 0), sizeof(shared), IPC_CREAT | 0666);
    if (shmid == -1) err("shmget");
    state = shmat(shmid, NULL, 0);

    state->barber = WORKING; 
    state->customers_waiting = sem_make_and_set(0);
    state->change_WR = sem_make_and_set(1);
    state->chair = sem_make_and_set(0);
    state->waiting_room = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    state->WR_places = sem_make_and_set(atoi(argv[1]));
    atexit(clean_up);

    barber();

    return EXIT_SUCCESS;
}

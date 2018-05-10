#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include "shared_barber.h"
#include "messages.h"

// ---------- Semaphores ---------------------------

shared state;

void sem_take (int semid) {
    struct sembuf op;
    op.sem_op = -1;
    op.sem_num = 0;
    op.sem_flag = 0;
    semop(semid, &op, 1);
}

void sem_give (int semid) {
    struct sembuf op;
    op.sem_op = 1;
    op.sem_num = 0;
    op.sem_flag = 0;
    semop(semid, &op, 1);
}


// ---------- Communication ------------------------

void err (const char *msg) {
    errno != 0 ? perror(msg) : printf(COLOR_RED"%s\n", msg);
    exit(EXIT_FAILURE);
}

void barber () {
    while (1) {
        log_message("I am falling asleep... zzz...");
        sem_take(state.customers_ready);
        log_message("Woken up!");
        sem_take(state.change_waiting_room);

        log_message("Please %d, come here.", pid);
        // take sth from FIFO
        sem_give(state.barber_ready);
        log_message("Mr %d, I am ready for the haircut.", pid);
        sem_give(state.change_waiting_room);
        log_message("Mr %d, I have finished your haircut.", pid);
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
    int shared(ftok(home_path, 0), sizeof(shared), IPC_CREAT | S_IWUSR | S_IRUSR);

    barber_ready = semget(ftok(home_path, 1), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    customers_ready = semget(ftok(home_path 2), 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    change_WR = semget(ftok(home_path 3), 1, IPC_CREAT | S_IWUSR | S_IRUSR);

    barber();

    return EXIT_SUCCESS;
}

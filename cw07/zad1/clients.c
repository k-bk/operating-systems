#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <stdarg.h>
#include "communication.h"

// ---------- Communication ------------------------

void log_message (const char *message, ... ) {
    struct timespec time_stamp;
    clock_gettime(CLOCK_MONOTONIC, &time_stamp);
    printf("%ld ", time_stamp.tv_nsec);

    va_list arg;
    va_start(arg, message);
    vprintf(message, arg);
    va_end(arg);

    printf("\n");
}

// ---------- Semaphores ---------------------------

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


// ---------- Main program -------------------------

int main (int argc, char **argv) {

    if (argc < 3) {
        printf(COLOR_RED "%s: not enough arguments\n" COLOR_RESET
               "Usage: '%s <num_of_chairs>'\n", argv[0], argv[0]);
    }

    barber_ready = semget(key_t key, 0);
    customers_ready = semget(key_t key, 0);
    change_waiting_room = semget(key_t key, 0);

    while (1) {
        sem_take(change_waiting_room);
        if (FIFO has free places) {
            LOG_MYSELF in FIFO
            sem_give(customers_ready);
            sem_give(change_waiting_room);
            sem_take(barber_ready);

        }
    }

    return EXIT_SUCCESS;
}

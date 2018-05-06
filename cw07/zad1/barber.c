#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <stdarg.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// ---------- Semaphores ---------------------------

int barber_ready;
int customers_ready;
int change_waiting_room; 

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

    if (argc < 2) {
        printf(ANSI_COLOR_RED "%s: not enough arguments\n" ANSI_COLOR_RESET
               "Usage: '%s <num_of_chairs>'\n", argv[0], argv[0]);
    }

    barber_ready = semget(key_t key, 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    customers_ready = semget(key_t key, 1, IPC_CREAT | S_IWUSR | S_IRUSR);
    change_waiting_room = semget(key_t key, 1, IPC_CREAT | S_IWUSR | S_IRUSR);

    while (1) {
        log_message("I am falling asleep... zzz...");
        sem_take(customers_ready);
        log_message("Woken up!");
        sem_take(change_waiting_room);
        log_message("Please %d, come here.", pid);
        // take sth from FIFO
        sem_give(barber_ready);
        log_message("Mr %d, I am ready for the haircut.", pid);
        sem_give(change_waiting_room);
        log_message("Mr %d, I have finished your haircut.", pid);
    }

    return EXIT_SUCCESS;
}

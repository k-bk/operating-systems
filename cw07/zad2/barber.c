#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include "shared_barber.h"
#include "messages.h"

// ---------- Semaphores ---------------------------
int shmid = 0;
void *addr;
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
    msgctl(state->waiting_room, IPC_RMID, NULL);
    sem_close(state->customers_waiting);
    sem_close(state->change_WR);
    sem_close(state->chair);
    sem_close(state->WR_places);
    if (munmap(addr, sizeof(shared)) == -1) perror("munmap");
    if (shm_unlink(SHM_PATH) == -1) perror("shm_unlink");
}

// ---------- Semaphore loop -----------------------

void barber () {
    id_msg first_client;
    while (1) {
        sem_take(state->change_WR);
        if (sem_getval(state->customers_waiting) == 0) {
            if (state->barber != ASLEEP) 
                log_message(C_YELLOW "\t I am falling asleep... zzz..." C_RESET);
            state->barber = ASLEEP;
            sem_give(state->change_WR);
        } else {
            sem_take(state->customers_waiting);
            if (state->barber == ASLEEP) 
                log_message(C_YELLOW "\t Woken up!" C_RESET);
            state->barber = WORKING;
            msgrcv(state->waiting_room, &first_client, sizeof(id_msg), 0, 0);
            sem_give(state->change_WR);
            kill(first_client.pid, SIGUSR1);
            log_message("%d\t Come  here.", first_client.pid);
            sem_take(state->change_WR);
            sem_give(state->chair);
            log_message("%d\t Start haircut.", first_client.pid);
            log_message("%d\t Ended haircut.", first_client.pid);
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

    shmid = shm_open(SHM_PATH, O_RDWR | O_CREAT, S_IRWXU);
    if (shmid == -1) 
        err("shm_open");

    if (ftruncate(shmid, sizeof(shared)) < 0) 
        err("ftruncate");

    state = mmap(NULL, sizeof(shared), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    state->barber = WORKING; 
    state->customers_waiting = 
        sem_open(customers_waiting_path, O_RDWR | O_CREAT, 0666, 0);
    state->change_WR =
        sem_open(change_WR_path, O_RDWR | O_CREAT, 0666, 1);
    state->chair =
        sem_open(chair_path, O_RDWR | O_CREAT, 0666, 0);
    state->waiting_room = 
        msgget(IPC_PRIVATE, IPC_CREAT | 0666);
    state->WR_places = 
        sem_open(WR_places_path, O_RDWR | O_CREAT, 0666, atoi(argv[1]));
    atexit(clean_up);

    //barber();

    return EXIT_SUCCESS;
}

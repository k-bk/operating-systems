#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "shared_barber.h"
#include "messages.h"

// ---------- Globals ------------------------------
shared *state;
int invited = 0;

// ---------- Utilities ----------------------------

void err (const char *msg) {
    errno != 0 ? perror(msg) : printf(COLOR_RED"%s\n", msg);
    exit(EXIT_FAILURE);
}

void use_sigint (int _) {
    exit(EXIT_FAILURE);
}

void use_sigusr1 (int _) {
    invited = 1;
}

// ---------- Semaphore loop -----------------------

void client (int haircuts_needed) {
    id_msg my_id;
    my_id.pid = getpid();
    int haircuts = 0;

    while (haircuts < haircuts_needed) {
        sem_take(state->change_waiting_room);
        msgsnd(state->waiting_room, &my_id, sizeof(id_msg), 0);
        sem_give(state->customers_ready);
        log_message("%d I am entering the waiting room", getpid());
        while (!invited);
        log_message("%d I was invited by barber!", getpid());
        sem_take(state->change_waiting_room);
        sem_take(state->barber_ready);
        sem_take(state->chair);
        sem_give(state->chair);
    }
}

// ---------- Main program -------------------------

int main (int argc, char **argv) {

    signal(SIGINT, use_sigint);
    if (argc < 3) {
        printf(COLOR_RED "%s: not enough arguments\n" COLOR_RESET
               "Usage: '%s <num_of_clients> <num_of_haircuts>'\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    int num_of_clients = atoi(argv[1]);
    int haircuts_needed = atoi(argv[2]);

    char *home_path = getenv("HOME");
    if (home_path == NULL) err("getenv");

    int shmid = shmget(ftok(home_path, 0), sizeof(shared), 0);
    state = shmat(shmid, NULL, 0);

    client(haircuts_needed);

    return EXIT_SUCCESS;
}

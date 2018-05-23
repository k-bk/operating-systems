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

// ---------- Globals ------------------------------
shared *state;
int invited = 0;
int shmid;

// ---------- Utilities ----------------------------
//
void clean_up () {
}

void err (const char *msg) {
    errno != 0 ? perror(msg) : printf(C_RED"%s\n", msg);
    exit(EXIT_FAILURE);
}

void use_sigint (int _) {
    exit(EXIT_FAILURE);
}

void use_sigusr1 (int _) {
    log_message("%d\t I was invited by barber!", getpid());
    invited = 1;
    signal(SIGUSR1, use_sigusr1);
}

// ---------- Semaphore loop -----------------------

void client (int haircuts_needed) {
    id_msg my_id;
    my_id.pid = getpid();
    my_id.mtype = 1;
    int haircuts = 0;

    while (haircuts < haircuts_needed) {
        sem_take(state->change_WR);
        if (state->barber == ASLEEP) {
            log_message("%d\t Wake up, barber!", getpid());
        } 
        if (sem_getval(state->WR_places) > 0) {
            sem_take(state->WR_places);
            sem_give(state->customers_waiting);
            log_message("%d\t I sit in the WR.", getpid());
            if (msgsnd(state->waiting_room, &my_id, msg_size, 0) == -1) 
                perror("msgsnd");
            sem_give(state->change_WR);
            while (!invited);
            sem_take(state->change_WR);
            sem_give(state->WR_places);
            sem_give(state->change_WR);
            sem_take(state->chair);
            log_message("%d\t I sit on a chair.", getpid());
            haircuts += 1;
            log_message("%d\t " C_GREEN "%d" C_RESET 
                    " haircuts done.", getpid(), haircuts);
        } else {
            log_message("%d\t No place in WR. Going to exit.", getpid());
            sem_give(state->change_WR);
        }
    }
    log_message("%d\t DONE! That's all for me.", getpid());
    exit(EXIT_SUCCESS);
}

// ---------- Main program -------------------------

int main (int argc, char **argv) {

    signal(SIGINT, use_sigint);
    signal(SIGUSR1, use_sigusr1);
    if (argc < 3) {
        printf(C_RED "%s: not enough arguments\n" C_RESET
               "Usage: '%s <num_of_clients> <num_of_haircuts>'\n", argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    int num_of_clients = atoi(argv[1]);
    int haircuts_needed = atoi(argv[2]);

    shmid = shm_open(SHM_PATH, O_RDWR | O_CREAT, S_IRWXU);
    if (shmid == -1) 
        err("shm_open");

    if (ftruncate(shmid, sizeof(shared)) < 0) 
        err("ftruncate");

    atexit(clean_up);

    for (int i = 0; i < num_of_clients; i++) {
        int pid = fork();
        if (pid == 0) {
            client(haircuts_needed);
        }
    }


    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void use_SIGINT (int);
void use_SIGTSTP (int);

static int L;
static int TYPE;
static char* prog_name;
pid_t child_pid = 0;
int child_recieved = 0;
int child_end = 0;
int parent_sent = 0;
int parent_recieved = 0;
int parent_end = 0;

void show_help () {
    printf("Usage: %s <Num of signals> <Type>\n"
            " TYPE 1 – parent sends signals without waiting\n"
            "      2 – parent waits for answer for each signal\n"
            "      3 – processes send real time signals\n"
           , prog_name);
    exit(EXIT_FAILURE);
}

void set_signal(int signum, void* handler) {
    struct sigaction act_SIGUSR1;
    act_SIGUSR1.sa_handler = handler;
    sigfillset(&act_SIGUSR1.sa_mask);
    act_SIGUSR1.sa_flags = 0;
    sigaction(signum, &act_SIGUSR1, NULL);
}

void child_exit() {
    printf("Child: \n"
           "  signals recieved: %d\n"
           , child_recieved
          );
    exit(EXIT_SUCCESS);
}

void child_SIGUSR1(int signum) {
    child_recieved += 1;
    kill(getppid(), SIGUSR1);
}

void child_SIGUSR2(int signum) {
    child_end = 1;
}

void child_setup() {
    set_signal(SIGUSR1, child_SIGUSR1);
    set_signal(SIGUSR2, child_SIGUSR2);
    child_recieved = 0;

    printf("Child (%d) setup done\n", getpid());
}

void parent_exit() {
    printf("Parent: \n"
           "  signals sent: %d\n"
           "  signals recieved: %d\n"
           , parent_sent
           , parent_recieved
          );
    kill(child_pid, SIGUSR2);
    exit(EXIT_SUCCESS);
}

void parent_SIGUSR1(int signum) {
    parent_recieved += 1;

    if (TYPE == 3) {
        if (parent_recieved < L) {
            parent_sent += 1;
            kill(child_pid, SIGUSR1);
        } else {
            parent_end = 1;
        }
    }
}

void parent_setup() {
    set_signal(SIGUSR1, parent_SIGUSR1);
    parent_sent = 0;
    parent_recieved = 0;

    printf("Parent (%d) setup done\n", getpid());
}

int main (int argc, char** argv) {

    // Read args
    prog_name = argv[0];
    if (argc == 3) {
        L = atoi(argv[1]);
        TYPE = atoi(argv[2]);
        if (TYPE < 1 || TYPE > 3) show_help();
    } else {
        show_help();
    }

    child_pid = fork();

    if (child_pid == -1) {
        perror("Cannot make a fork");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        // Child fork
        child_setup();
    } else {
        // Parent fork
        parent_setup();
        switch (TYPE) {
            case 1:
                while (parent_sent < L) {
                    kill(child_pid, SIGUSR1);
                    parent_sent += 1;
                }
                parent_end = 1;
                break;
            case 2:
                break;
            case 3:
                kill(child_pid, SIGUSR1);
                parent_sent = 1;
                break;
        }
    }

    while (1) {
        if (child_end) child_exit();
        if (parent_end) parent_exit();
        pause();
    }

    return EXIT_SUCCESS;
}

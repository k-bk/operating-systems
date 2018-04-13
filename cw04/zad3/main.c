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
int parent_sent = 0;
int parent_recieved = 0;

void show_help () {
    printf("Usage: %s <Num of signals> <Type>\n"
           , prog_name);
    exit(EXIT_FAILURE);
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
    kill(getppid(), SIGUSR2);
    child_exit();
}

void child_setup() {
    struct sigaction act_SIGUSR1;
    act_SIGUSR1.sa_handler = child_SIGUSR1;
    sigfillset(&act_SIGUSR1.sa_mask);
    sigdelset(&act_SIGUSR1.sa_mask, SIGUSR1);
    act_SIGUSR1.sa_flags = 0;
    sigaction(SIGUSR1, &act_SIGUSR1, NULL);

    struct sigaction act_SIGUSR2;
    act_SIGUSR2.sa_handler = child_SIGUSR2;
    sigfillset(&act_SIGUSR2.sa_mask);
    sigdelset(&act_SIGUSR2.sa_mask, SIGUSR2);
    act_SIGUSR2.sa_flags = 0;
    sigaction(SIGUSR2, &act_SIGUSR2, NULL);

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
    if (parent_recieved < L) {
        parent_sent += 1;
        kill(child_pid, SIGUSR1);
    } else {
        parent_exit();
    }
}

void parent_setup() {
    struct sigaction act_SIGUSR1;
    act_SIGUSR1.sa_handler = parent_SIGUSR1;
    sigemptyset(&act_SIGUSR1.sa_mask);
    sigaddset(&act_SIGUSR1.sa_mask, SIGUSR2);
    act_SIGUSR1.sa_flags = 0;
    sigaction(SIGUSR1, &act_SIGUSR1, NULL);

    printf("Parent (%d) setup done\n", getpid());
    kill(child_pid, SIGUSR1);
    parent_sent = 1;
}

int main (int argc, char** argv) {

    prog_name = argv[0];
    if (argc == 3) {
        L = atoi(argv[1]);
        TYPE = atoi(argv[2]);
    } else {
        show_help();
    }

    child_pid = fork();

    if (child_pid == -1) {
        perror("Cannot make a fork");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
        child_setup();
    } else {
        parent_setup();
    }

    // Do nothing
    while (1) pause();
    return EXIT_SUCCESS;
}

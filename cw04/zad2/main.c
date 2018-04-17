#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define SIGRT_RANGE (SIGRTMAX - SIGRTMIN + 1)

typedef struct child {
    pid_t pid;
    int has_requested;
    int exit_code;
    int signum;
} child;

int debug = 1;
static int N;
static int M;
static char* prog_name;
child* children;
int requests_recieved = 0;
int first_batch_run = 0;
int children_alive = 0;
int type;

void show_help () {
    printf("Usage: %s <Number of children> <Number of requests>\n"
           , prog_name);
    exit(EXIT_FAILURE);
}

void set_signal(int signum, void* handler) {
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(signum, &act, NULL);
}

void child_SIGUSR1(int signum) {
    type = rand() % SIGRT_RANGE;
    kill(getppid(), SIGRTMIN+type);
}

int child_run() {
    srand(time(NULL) + getpid());
    set_signal(SIGUSR1, child_SIGUSR1);
    int sleep_time = rand() % 10 + 1;
    if (debug) printf("(%d) sleep: %d\n", getpid(), sleep_time);
    sleep(sleep_time);
    kill(getppid(), SIGUSR1);
    pause();
    if (debug) printf("(%d) sending SIGRT+%d\n", getpid(), type);
    exit(sleep_time);
}

void parent_exit(int signum) {
    for (int i = 0; i < N; i++) {
        if (children[i].pid)
            kill(children[i].pid, SIGKILL);
        if (children[i].exit_code) {
            printf("(%d) exited with code %d\n", children[i].pid, children[i].exit_code); 
        }
    }
    exit(EXIT_SUCCESS);
}

void parent_SIGUSR1(int signum, siginfo_t* info, void* context) {
    if (debug) printf("Request from (%d)\n", info->si_pid);
    for (int i = 0; i < N; i++) {
        if (children[i].pid == info->si_pid) {
            children[i].has_requested = 1;
            requests_recieved += 1;
            if (first_batch_run) {
                if (debug) printf("(%d) permission given\n", children[i].pid);
                kill(children[i].pid, SIGUSR1);
            }
            break;
        }
    }
    if (!first_batch_run && requests_recieved >= M) {
        first_batch_run = 1;
        for (int i = 0; i < N; i++) {
            if (children[i].has_requested) {
                if (debug) printf("(%d) permission given\n", children[i].pid);
                kill(children[i].pid, SIGUSR1);
            }
        }
    }
}

void parent_SIGCHLD(int signum, siginfo_t* info, void* context) {
    children_alive -= 1;
    int i;
    for (i = 0; i < N; i++)
        if (children[i].pid == info->si_pid) break;
    int exit_code;
    waitpid(children[i].pid, &exit_code, WNOHANG);
    children[i].exit_code = exit_code;
}

void parent_SIGRT(int signum, siginfo_t* info, void* context) {
    printf("SIGRT+%d from (%d)\n", signum - SIGRTMIN, info->si_pid);
    for (int i = 0; i < N; i++) {
        if (children[i].pid == info->si_pid)
            children[i].signum = signum;
    }
}

void parent_run() {
    struct sigaction act;
    act.sa_sigaction = parent_SIGRT;  
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    for (int i = 0; i < SIGRT_RANGE; i++) {
        sigaction(SIGRTMIN+i, &act, NULL);
    }
    act.sa_sigaction = parent_SIGUSR1;  
    sigaction(SIGUSR1, &act, NULL);
    act.sa_sigaction = parent_SIGCHLD;  
    sigaction(SIGCHLD, &act, NULL);
    set_signal(SIGINT, parent_exit);
    first_batch_run = 0;
    if (debug) printf("(%d) Parent setup done\n", getpid());
    while (1) {
        if (children_alive <= 0) parent_exit(1);
        pause();
    }
}


int main (int argc, char** argv) {
    // Read args
    prog_name = argv[0];
    if (argc == 3) {
        N = atoi(argv[1]);
        M = atoi(argv[2]);
    } else {
        show_help();
    }

    pid_t child_pid;
    children = calloc(N, sizeof(child));

    for (int i = 0; i < N; i++) {
        child_pid = fork();
        if (child_pid == -1) {
            perror("Cannot make a fork. Aborting.\n");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) {
            child_run();
            break;
        } else {
            if (debug) printf("(%d) child forked\n", child_pid);
            children_alive += 1;
            children[i].pid = child_pid;
        }
    }
    parent_run();

    while (1) {
        pause();
    }

    return EXIT_SUCCESS;
}

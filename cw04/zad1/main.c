#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

void use_SIGINT (int);
void use_SIGTSTP (int);

int show_date = 1;
int child_pid;

void run () {
    time_t current_time;
    while (1) {
        if (show_date) {
            time(&current_time);
            printf("%s", ctime(&current_time));
        }
        sleep(1);
    }
}

void bash_run () {
    child_pid = fork();
    if (child_pid == -1) {
        perror("Cannot make a fork");
        exit(EXIT_FAILURE);
    } 
    if (child_pid == 0) {
        execlp("./date_loop.sh", "date_loop.sh", NULL);
        exit(EXIT_SUCCESS);
    }
}

void use_SIGTSTP (int signum) {
    show_date = !show_date;
    if (!show_date) {
        printf("\n Waiting for CTRL+Z – continue or CTRL+C – end program\n");
#ifdef BASH
        kill(child_pid, SIGSTOP);
    } else {
        bash_run();
#endif
    }
}

void use_SIGINT (int signum) {
    printf("\n");
#ifdef BASH
    kill(child_pid, SIGSTOP);
#endif
    exit(EXIT_SUCCESS);
}

int main () {
    struct sigaction act;
    act.sa_handler = use_SIGINT;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    signal(SIGTSTP, use_SIGTSTP);
#ifdef BASH
    printf("Running bash version\n");
    bash_run();
#else
    run();
#endif
    while (1) pause();
    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

void time_loop () {
    time_t current_time;
    while (1) {
	time(&current_time);
	printf("%s", ctime(&current_time));
	sleep(1);
    }
}

void use_SIGTSTP () {
    printf("Waiting for CTRL+Z – continue or CTRL+C – end program\n");
    signal(SIGTSTP, time_loop);
    while (1) {
	sleep(1);
    }
}

void use_SIGINT (int signum) {
    printf("Recieved SIGINT signal\n");
    exit(EXIT_SUCCESS);
}

void use_SIGCONT () {
    printf("Recieved SIGCONT, working again.\n");
    time_loop();
}

int main () {
    struct sigaction act;
    act.sa_handler = use_SIGTSTP;
    act.sa_mask = SIGINT;
    act.sa_flags = 0;

    sigaction(SIGTSTP, &act, NULL);
    signal(SIGINT, use_SIGINT);
    signal(SIGCONT, use_SIGCONT);
    time_loop();
    return EXIT_SUCCESS;
}

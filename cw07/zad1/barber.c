#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <stdarg.h>

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

void state_sleeping () {
    log_message("Falling asleep... zzz...");
    while (1) {
    }
}

void state_shaving (int pid) {
    log_message("Starting the shave of %d.", pid);
    while (1) {
    }
    log_message("Ending the shave of %d.", pid);
}

// ---------- Main program -------------------------
int main () {
    return EXIT_SUCCESS;
}

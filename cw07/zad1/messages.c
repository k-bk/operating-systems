#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/sem.h>
#include "messages.h"

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
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

void sem_give (int semid) {
    struct sembuf op;
    op.sem_op = 1;
    op.sem_num = 0;
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/sem.h>
#include <signal.h>
#include <errno.h>
#include "messages.h"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

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

int sem_make () {
    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (semid == -1) perror("sem_make");
    return semid;
}
void sem_take (const int semid) {
    struct sembuf op;
    op.sem_op = -1;
    op.sem_num = 0;
    op.sem_flg = 0;
    if (semop(semid, &op, 1) == -1) {
        if (errno == EIDRM) raise(SIGINT);
    }
}

void sem_give (const int semid) {
    struct sembuf op;
    op.sem_op = 1;
    op.sem_num = 0;
    op.sem_flg = 0;
    semop(semid, &op, 1);
}

void sem_set (const int semid, const int value) {
    union semun init;
    init.val = value;
    semctl(semid, 0, SETVAL, init);
}

int sem_make_and_set (const int value) {
    int semid = sem_make();
    sem_set(semid, value);
    return semid;
}

int sem_getval (const int semid) {
    return semctl(semid, 0, GETVAL, 0);
}

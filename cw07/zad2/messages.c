#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <errno.h>
#include "messages.h"

void log_message (const char *message, ... ) {
    struct timespec time_stamp;
    clock_gettime(CLOCK_MONOTONIC, &time_stamp);
    printf("%ld \t", time_stamp.tv_nsec);

    va_list arg;
    va_start(arg, message);
    vprintf(message, arg);
    va_end(arg);

    printf("\n");
}

sem_t *sem_make (int shmid) {
    sem_t *semid = mmap(NULL, sizeof(sem_t), 
            PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (semid == MAP_FAILED) perror("sem_make");
    return semid;
}

void sem_take (sem_t *sem) {
    if (sem_wait(sem) == -1) perror("sem_wait");
}

void sem_give (sem_t *sem) {
    if (sem_post(sem) == -1) perror("sem_wait");
}

void sem_set (sem_t* sem, const int value) {
    if (sem_init(sem, 1, value) == -1) 
        perror("sem_init");
}

sem_t *sem_make_and_set (const int shmid, const int value) {
    sem_t *sem = sem_make(shmid);
    sem_set(sem, value);
    return sem;
}

int sem_getval (sem_t *sem) {
    int val;
    if (sem_getvalue(sem, &val) == -1) perror("get_value");
    return val;
}

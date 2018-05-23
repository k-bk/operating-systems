#ifndef MESSAGES
#define MESSAGES

#define C_RED     "\x1b[31m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m"
#define C_BLUE    "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN    "\x1b[36m"
#define C_RESET   "\x1b[0m"


#include <stdarg.h>

extern void log_message (const char *message, ...);
extern sem_t *sem_make (int shmid);
extern sem_t *sem_make_and_set (int shmid, int value);
extern void sem_take (sem_t *sem);
extern void sem_give (sem_t *sem);
extern int sem_getval (sem_t *sem);
extern void sem_set (sem_t *sem, int value);

#endif

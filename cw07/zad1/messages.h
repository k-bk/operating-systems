#ifndef MESSAGES
#define MESSAGES

#include <stdarg.h>

extern void log_message (const char *message, ...);
extern int sem_make ();
extern int sem_make_and_set (int value);
extern void sem_take (int semid);
extern void sem_give (int semid);
extern int sem_getval (int semid);
extern void sem_set (int semid, int value);

#endif

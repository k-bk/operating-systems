#ifndef MESSAGES
#define MESSAGES

#include <stdarg.h>

extern void log_message (const char *message, ...);
extern void sem_take (int semid);
extern void sem_give (int semid);

#endif

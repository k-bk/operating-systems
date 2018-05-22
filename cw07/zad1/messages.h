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
extern int sem_make ();
extern int sem_make_and_set (int value);
extern void sem_take (int semid);
extern void sem_give (int semid);
extern int sem_getval (int semid);
extern void sem_set (int semid, int value);

#endif

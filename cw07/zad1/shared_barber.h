#ifndef SHARED_BARBER
#define SHARED_BARBER

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

typedef struct shared {
    int waiting_room; // fifo
    int barber_ready;
    int chair;
    int customers_ready;
    int change_waiting_room; 
} shared;

typedef struct id_msg {
    long mtype;
    int pid;
} id_msg;

#endif

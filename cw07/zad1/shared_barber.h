#ifndef SHARED_BARBER
#define SHARED_BARBER

enum b_state { ASLEEP = 1, WORKING };

typedef struct shared {
    int waiting_room; // fifo
    int WR_places;    // free places in fifo
    int change_WR;
    int customers_waiting;
    int chair;
    enum b_state barber;
} shared;

typedef struct id_msg {
    long mtype;
    int pid;
} id_msg;

const size_t msg_size = sizeof(id_msg);

#endif

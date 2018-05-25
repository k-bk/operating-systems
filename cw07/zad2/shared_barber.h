#ifndef SHARED_BARBER
#define SHARED_BARBER

enum b_state { ASLEEP = 1, WORKING };

typedef struct shared {
    int waiting_room; // fifo
    sem_t *WR_places;    // free places in fifo
    sem_t *change_WR;
    sem_t *customers_waiting;
    sem_t *chair;
    enum b_state barber;
} shared;

typedef struct id_msg {
    long mtype;
    int pid;
} id_msg;

const size_t msg_size = sizeof(id_msg);
const char *SHM_PATH = "shm_barber";
const char *WR_places_path = "WR_places";
const char *change_WR_path = "change_WR";
const char *customers_waiting_path = "customers_waiting";
const char *chair_path = "chair";

#endif

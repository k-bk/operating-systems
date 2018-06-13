#ifndef COMMUNICATION_H 
#define COMMUNICATION_H

#define MAX_CLIENT_NAME 15

typedef struct task_t {
    int task_id;
    char client_name [MAX_CLIENT_NAME + 1];
    char op [4]; 
    double arg1;
    double arg2;
} task_t;

#endif

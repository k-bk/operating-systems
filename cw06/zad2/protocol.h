#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#define MIRROR  1
#define ADD     2
#define SUB     4
#define MUL     6
#define DIV     7
#define TIME    8
#define STOP    9
#define START   10 
#define MAX_CLIENTS 20 
#define MAX_MSGLEN 100
#define PROJECT_ID 7 

typedef struct message {
    int id; 
    int cmd;
    char args[MAX_MSGLEN];
} message;

const size_t MESSAGE_SIZE = sizeof(message);
const char* SERVER_NAME = "/server";

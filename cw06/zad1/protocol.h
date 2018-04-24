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
#define END     9
#define START   10 

typedef struct my_msgbuf {
    long type;
    int id; 
    int cmd;
    char args[1];
} my_msgbuf;

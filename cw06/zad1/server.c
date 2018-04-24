#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <string.h>
#include "protocol.h"

typedef struct client {
    int id;
    int qid;
} client;

client clients [20];
int client_count = 0;

void free_resources () {
    printf ("ending everything\n");
}

int my_msgsnd(client client, char* message) {
    int msglen = strlen(message);
    my_msgbuf* buffer = malloc(sizeof(my_msgbuf) + msglen);
    buffer->id = client.id; 
    buffer->type = 1;
    strcpy(buffer->args, message);
    printf("(message) id: %d qid: %d args: %s\n", buffer->id, client.qid, buffer->args);
    int status = msgsnd(client.qid, buffer, sizeof(buffer), 0);
    free (buffer);
    return status;
}

double calculate (int operator, char* args) {

    double acc;
    if (operator == ADD || operator == SUB) acc = 0.0;
    else acc = 1.0;

    char* args_copy = strdup(args);
    char* arg = NULL;
    while ((arg = strsep(&args_copy, " \n")) != NULL) {
        if (strlen(arg) > 0) {
            printf("(calc) %s\n", arg);
            switch (operator) {
                case ADD: acc += atof(arg); break;
                case SUB: acc -= atof(arg); break;
                case MUL: acc *= atof(arg); break;
                case DIV: acc /= atof(arg);
            }
        }
    }
    return acc;
}

char* mirror (char* args) {
    int len = strlen(args);
    char* result = malloc(len);
    for (int i = 0; i < len; i++) {
        result[i] = args[len-i-1];
    }
    printf("(mirror) %s\n", result);
    return result;
}

void add_client (int id, char* args) {
    int key = atoi(args);
    int qid = msgget(key, 0);
    if (qid == -1) 
        perror("using given key");
    client_count += 1;
    clients[id].id = id;
    clients[id].qid = qid;
    if (my_msgsnd(clients[id], "") == -1) 
        perror("add_client");
}

int main () {

    atexit (free_resources);
    key_t key = ftok ("server.c", 'a');
    printf("(server) key: %d\n", key);
    int qid = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(qid == -1)
        perror("opening the FIFO");

    int id_gen = 0;
    int msglen = 1000;
    my_msgbuf* buffer = malloc(sizeof(my_msgbuf) + msglen);
    buffer->type = 1;

    while (1) {
        msgrcv(qid, buffer, msglen, 0, 0); 
        printf("(recieve) id: %d cmd: %d args: %s\n", buffer->id, buffer->cmd, buffer->args);
        double result;
        switch (buffer->cmd) {
            case START:
                printf("(server) Adding client\n");
                add_client(id_gen++, buffer->args);
                break;
            case END:
                printf("(server) Received END signal, aborting...\n");
                break;
            case ADD: case SUB: case MUL: case DIV:
                result = calculate(buffer->cmd, buffer->args);
                sprintf(buffer->args, "%.2f", result);
                printf("(message) id: %d qid: %d args: %s\n", buffer->id, clients[buffer->id].qid, buffer->args);
                if (msgsnd(clients[buffer->id].qid, buffer, msglen, 0) == -1) perror("sending result");
                break;
            case MIRROR:
                my_msgsnd(clients[buffer->id], mirror(buffer->args));
                break;
            default:
                printf("(server) Got wrong message\n");
        }
    }
    exit (EXIT_SUCCESS);
}

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

int my_msgsnd(client client, char *message) {
    printf("client: %d %d\n", client.id, client.qid);
    int msglen = strlen(message);
    my_msgbuf *buffer = malloc(sizeof(my_msgbuf) + msglen);
    buffer->id = client.id; 
    buffer->type = 1;
    int status = msgsnd(client.qid, buffer, sizeof(buffer), 0);
    free (buffer);
    return status;
}

double calculate (int operator, char* args) {
    double acc;
    if (operator == ADD || operator == SUB) acc = 0.0;
    else acc = 1.0;

    char* arg;
    while ((arg = strtok(args, " \n")) != NULL) {
        switch (operator) {
            case ADD: acc += atof(arg); break;
            case SUB: acc -= atof(arg); break;
            case MUL: acc *= atof(arg); break;
            case DIV: acc /= atof(arg);
        }
    }
    return acc;
}

char *mirror (char *args) {
    int len = strlen(args);
    char *result = malloc(len);
    for (int i = 0; i < len; i++) {
        result[i] = args[len-i-1];
    }
    return result;
}

void add_client (int id, char* args) {
    int key = atoi(args);
    int qid = msgget(key, 0);
    if (qid == -1) 
        perror("using given key");
    client_count += 1;
    clients[client_count].id = id;
    clients[client_count].qid = qid;
    printf("sending id: %d to queue: %d\n", id, qid);
    if (my_msgsnd(clients[client_count], "") == -1) 
        perror("add_client");
}

client client_with_id (int id) {
    client client;
    for (int i = 0; i < client_count; i++) {
        if (clients[i].id == id) client = clients[i]; 
    }
    return client;
}

int main () {

    atexit (free_resources);
    key_t key = ftok ("server.c", 'a');
    printf("key: %d\n", key);
    int qid = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(qid == -1)
        perror("opening the FIFO");

    int id_gen = 0;
    int msglen = 1000;
    my_msgbuf *buffer = malloc(sizeof(my_msgbuf) + msglen);
    buffer->type = 1;

    while (1) {
        msgrcv(qid, buffer, msglen, 0, 0); 
        printf("id: %d cmd: %d args: %s\n", buffer->id, buffer->cmd, buffer->args);
        switch (buffer->cmd) {
            case START:
                add_client(id_gen++, buffer->args);
                break;
            case END:
                printf("Received END signal, aborting...\n");
                break;
            case ADD: case SUB: case MUL: case DIV:
                printf("Sending result");
                sprintf(buffer->args, "%.2f", calculate(buffer->cmd, buffer->args));
                my_msgsnd(client_with_id(buffer->id), buffer->args);
                break;
            case MIRROR:
                printf("mirror: %s\n", mirror(buffer->args));
                my_msgsnd(client_with_id(buffer->id), mirror(buffer->args));
        }
    }
    exit (EXIT_SUCCESS);
}

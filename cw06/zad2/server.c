#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <mqueue.h>
#include "protocol.h"

typedef struct client {
    int id;
    int des;
} client;

void free_resources ();
int find_queue_with_id ();
double calculate (int operator, char* args);
int message_send(int id, int cmd, char* args);

client clients [MAX_CLIENTS];
int client_count = 0;
int server_des = -1;
unsigned int priority;

// --------- Resource management ------------------------------

void free_resources () {
    if (server_des != -1) {
	if(mq_close(server_des)) perror("closing server_des");
    }
    for (int i = 0; i < client_count; i++) {
	mq_close();
    }
    printf("(client) Queue %d closed.\n", private_des, server_des);
    mq_unlink(SERVER_NAME);
}

void use_SIGINT (int signum) {
    printf("(server) Recieved SIGINT.\n");
    exit(EXIT_FAILURE);
}

// --------- Server configuration -----------------------------

int find_queue_with_id(int id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].id == id) return clients[i].des;
    }
    return -1;
}

int message_send(int id, int cmd, char* args) {
    message buffer;
    buffer.id = id; 
    //strcpy(buffer.args, args);
    sprintf(buffer.args, "%s", args);
    int des = find_queue_with_id(id);
    printf("(message) id: %d des: %d args: %s\n", buffer.id, des, buffer.args);
    int status = mq_send(des, (char*) &buffer, MESSAGE_SIZE, 1);
    return status;
}

void add_client (int id, char* args) {
    if (client_count >= MAX_CLIENTS) {
        printf("(server) Client limit reached.\n");
        return;
    }

    int key = atoi(args);
    int des = msgget(key, 0);
    if (des == -1) {
        perror("using given key");
        exit(EXIT_FAILURE);
    }

    clients[client_count].id = id;
    clients[client_count].des = des;
    if (message_send(id, START, "HELLO") == -1) {
        perror("add_client");
        exit(EXIT_FAILURE);
    }
    client_count += 1;
}

void delete_client (int id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].id == id) {
            clients[i].id = clients[client_count].id;
            clients[i].des = clients[client_count].des;
            client_count -= 1;
        }
    }
}

// --------- Command processing -------------------------------

double calculate (int operator, char* args) {
    double acc;
    int first = 1;

    char* args_copy = strdup(args);
    char* arg = NULL;
    while ((arg = strsep(&args_copy, " \n")) != NULL) {
        if (strlen(arg) > 0) {
            if (first) {
                acc = atof(arg);
                first = 0;
            } else {
                printf("(calc) %s\n", arg);
                switch (operator) {
                    case ADD: acc += atof(arg); break;
                    case SUB: acc -= atof(arg); break;
                    case MUL: acc *= atof(arg); break;
                    case DIV: acc /= atof(arg); break;
                }
            }
        }
    }
    return acc;
}

void timer (char* message) {
    time_t current_time;
    time(&current_time);
    sprintf(message, "%s", ctime(&current_time));
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

// --------- Main program -------------------------------------

int main (int argc, char** argv) {
    if (atexit (free_resources) != 0) {
        perror("setting atexit");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, use_SIGINT) < 0) {
        perror("setting signal handler");
        exit(EXIT_FAILURE);
    }
    printf("(server) initialization...\n");
    server_des = mq_open(SERVER_NAME, O_CREAT | O_RDWR, NULL);
    if(server_des == -1) {
        perror("opening the FIFO");
        exit(EXIT_FAILURE);
    }
    printf("(server) server_des = %d\n", server_des);

    int id_gen = 0;
    message buffer; 
    double result = 0;
    char result_str [100];

    while (1) {
        if (mq_receive(server_des, (char*) &buffer, MESSAGE_SIZE, &priority) == -1) perror("receive");
        printf("(receive) id: %d cmd: %d args: %s\n", buffer.id, buffer.cmd, buffer.args);
        switch (buffer.cmd) {
            case START: 
                printf("(add)\n");
                add_client(id_gen++, buffer.args);
                break;
            case ADD: case SUB: case MUL: case DIV:
                printf("(calc)\n");
                result = calculate(buffer.cmd, buffer.args);
                sprintf(result_str, "%.2f", result);
                message_send(buffer.id, -1, result_str);
                break;
            case MIRROR:
                message_send(buffer.id, -1, mirror(buffer.args));
                break;
            case TIME:
                timer(result_str);
                message_send(buffer.id, -1, result_str);
                break;
        }
    }
    exit (EXIT_SUCCESS);
}

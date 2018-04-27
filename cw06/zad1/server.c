#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "protocol.h"

typedef struct client {
    int id;
    int qid;
} client;

void free_resources ();
int find_queue_with_id ();
double calculate (int operator, char* args);
int message_send(int id, int cmd, char* args);

client clients [MAX_CLIENTS];
int client_count = 0;
int server_qid = -1;

// --------- Resource management ------------------------------

void free_resources () {
    if (server_qid == -1) return;
    if (msgctl(server_qid, IPC_RMID, NULL) == -1) {
        perror("deleting the queue");
        return;
    }
    printf("(server) Queue %d closed.\n", server_qid);
}

void use_SIGINT (int signum) {
    printf("(server) Recieved SIGINT.\n");
    exit(EXIT_FAILURE);
}

// --------- Server configuration -----------------------------

int find_queue_with_id(int id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].id == id) return clients[i].qid;
    }
    return -1;
}

int message_send(int id, int cmd, char* args) {
    message buffer;
    buffer.type = 1;
    buffer.id = id; 
    //strcpy(buffer.args, args);
    sprintf(buffer.args, "%s", args);
    int qid = find_queue_with_id(id);
    printf("(message) id: %d qid: %d args: %s\n", buffer.id, qid, buffer.args);
    return msgsnd(qid, &buffer, MESSAGE_SIZE, 0);
}

void add_client (int id, char* args) {
    if (client_count >= MAX_CLIENTS) {
        printf("(server) Client limit reached.\n");
        return;
    }

    int key = atoi(args);
    int qid = msgget(key, 0);
    if (qid == -1) {
        perror("using given key");
        exit(EXIT_FAILURE);
    }

    clients[client_count].id = id;
    clients[client_count].qid = qid;
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
            clients[i].qid = clients[client_count].qid;
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
    char* home_path = getenv("HOME");
    if (home_path == NULL) {
        perror("getenv");
        exit(EXIT_FAILURE);
    }
    key_t key = ftok (home_path, PROJECT_ID);
    printf("(server) key: %d\n", key);
    if (key == -1) {
        perror("generating ftok");
        exit(EXIT_FAILURE);
    }
    server_qid = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(server_qid == -1) {
        perror("opening the FIFO");
        exit(EXIT_FAILURE);
    }
    printf("(server) server_qid = %d\n", server_qid);

    int id_gen = 0;
    message buffer; 
    buffer.type = 1;
    double result = 0;
    char result_str [100];

    while (1) {
        if (msgrcv(server_qid, &buffer, MESSAGE_SIZE, 0, 0) == -1) perror("recieving");
        printf("(recieve) id: %d cmd: %d args: %s\n", buffer.id, buffer.cmd, buffer.args);
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

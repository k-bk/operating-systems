#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include "protocol.h"

int private_qid = -1;
int server_qid = -1;

void free_resources () {
    if (private_qid == -1) return;
    if (msgctl(private_qid, IPC_RMID, NULL) == -1) {
        perror("deleting the queue");
        return;
    }
    printf("(client) Queue %d closed.\n", private_qid);
}

void use_SIGINT (int signum) {
    printf("(client) Recieved SIGINT.\n");
    exit(EXIT_FAILURE);
}

int command_to_int (char* command) {
    if (strcmp(command, "MIRROR") == 0) return MIRROR;
    if (strcmp(command, "ADD") == 0) return ADD;
    if (strcmp(command, "SUB") == 0) return SUB;
    if (strcmp(command, "MUL") == 0) return MUL;
    if (strcmp(command, "DIV") == 0) return DIV;
    if (strcmp(command, "START") == 0) return START;
    if (strcmp(command, "STOP") == 0) return STOP;
    if (strcmp(command, "TIME") == 0) return TIME;
    return -1; 
}

int main (int argc, char** argv) {
    if (atexit (free_resources) != 0) {
        perror("setting atexit");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, use_SIGINT) < 0) {
        perror("setting signal handler");
        exit(EXIT_FAILURE);
    }
    printf("(client) initialization...\n");
    char* home_path = getenv("HOME");
    if (home_path == NULL) {
        perror("getenv");
        exit(EXIT_FAILURE);
    }
    key_t key = ftok (home_path, PROJECT_ID);
    key_t private_key = key + getpid(); 
    printf("(client) key: %d\n", key);
    if (key == -1) {
        perror("generating ftok");
        exit(EXIT_FAILURE);
    }
    printf("(client) Waiting for server to connect...\n");
    server_qid = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);
    private_qid = msgget(private_key, IPC_CREAT | S_IRUSR | S_IWUSR);
    if(server_qid == -1 || private_qid == -1) {
        perror("opening the FIFO");
        exit(EXIT_FAILURE);
    }
    printf("(client) server_qid = %d\n", server_qid);
    printf("(client) private_qid = %d\n", private_qid);

    message recieve_buffer; 
    message buffer; 
    buffer.type = 1;
    buffer.cmd = START;
    buffer.id = getpid();
    sprintf(buffer.args, "%d", private_key);
    msgsnd(server_qid, &buffer, MESSAGE_SIZE, 0);

    if (msgrcv(private_qid, &recieve_buffer, MESSAGE_SIZE, 0, 0) == -1) {
        perror("recieving");
        exit(EXIT_FAILURE);
    }
    printf("(recieve) id: %d cmd: %d args: %s\n", recieve_buffer.id, recieve_buffer.cmd, recieve_buffer.args);
    buffer.id = recieve_buffer.id;

    printf("Welcome to HyperCalc2000. Type [MIRROR|ADD|SUB|MUL|DIV|END] <args>\n");

    char command [15];
    while(1) {
        printf("> ");
        if (scanf("%s", command) == -1) continue;
        //printf("command = %s, args = %s\n", command, buffer.args);
        int cmd = command_to_int(command);
        if (cmd == -1) printf("'%s' is a wrong command.\n", command);
        buffer.cmd = cmd;
        switch (cmd) {
            case MIRROR:
                scanf("%s", buffer.args);
                break;
            case ADD: case SUB: case MUL: case DIV:
                scanf("%[-0-9 ]", buffer.args); 
                break;
            case TIME: 
                break;
        }
        if (msgsnd(server_qid, &buffer, MESSAGE_SIZE, 0)) {
            perror("sending the command");
            exit(EXIT_FAILURE);
        }
        if (msgrcv(private_qid, &recieve_buffer, MESSAGE_SIZE, 0, 0) == -1) {
            perror("recieving");
            exit(EXIT_FAILURE);
        }
        printf(" = %s\n", recieve_buffer.args);
        //printf("(recieve) id: %d cmd: %d args: %s\n", recieve_buffer.id, recieve_buffer.cmd, recieve_buffer.args);
    }
    exit (EXIT_SUCCESS);
}

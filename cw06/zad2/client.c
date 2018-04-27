#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "protocol.h"

mqd_t private_des = -1;
mqd_t server_des = -1;
const char PRIVATE_NAME = "/client";

void free_resources () {
    if (server_des != -1) {

	if(mq_close(server_des)) perror("closing server_des");
    }
    if (private_des != -1) 
	if(mq_close(private_des)) perror("closing private_des");
    printf("(client) Queue %d closed.\n", private_des, server_des);
    mq_unlink(PRIVATE_NAME);
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

    key_t private_key = key + getpid(); 
    int piority;
    printf("(client) Waiting for server to connect...\n");
    server_des = mq_open(SERVER_NAME, O_CREAT | O_RDWR, NULL);
    private_des = mq_open("/client", O_RDWR, NULL);
    if(server_des == -1 || private_des == -1) {
        perror("opening the FIFO");
        exit(EXIT_FAILURE);
    }
    printf("(client) server_des = %d\n", server_des);
    printf("(client) private_des = %d\n", private_des);

    message recieve_buffer [MESSAGE_SIZE]; 
    message buffer [MESSAGE_SIZE]; 
    buffer.cmd = START;
    buffer.id = getpid();
    sprintf(buffer.args, "%d", private_des);
    if (mq_send(server_des, buffer, MESSAGE_SIZE, 1) == -1) {
        perror("sending");
	exit(EXIT_FAILURE);
    }
    if (mq_recieve(private_des, recieve_buffer, MESSAGE_SIZE, &priority) == -1) {
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
        if (mq_send(server_des, buffer, MESSAGE_SIZE, 1) == -1) {
            perror("sending the command");
            exit(EXIT_FAILURE);
        }
        if (mq_recieve(private_des, recieve_buffer, MESSAGE_SIZE, &priority) == -1) {
            perror("recieving");
            exit(EXIT_FAILURE);
        }
        printf(" = %s\n", recieve_buffer.args);
    }
    exit (EXIT_SUCCESS);
}

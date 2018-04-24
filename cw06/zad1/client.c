#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "protocol.h"

void free_resources () {
    printf ("ending everything\n");
}

int command_to_int (char* command) {
    if (strcmp(command, "MIRROR") == 0) return MIRROR;
    if (strcmp(command, "ADD") == 0) return ADD;
    if (strcmp(command, "SUB") == 0) return SUB;
    if (strcmp(command, "MUL") == 0) return MUL;
    if (strcmp(command, "DIV") == 0) return DIV;
    if (strcmp(command, "START") == 0) return START;
    if (strcmp(command, "END") == 0) return END;
    return -1; 
}

int main () {
    srand(time(NULL));
    atexit (free_resources);

    key_t srv_key = ftok("server.c", 'a');
    int srv_qid = msgget(srv_key, 0);
    if (srv_qid == -1) perror("opening the FIFO");
    
    key_t key = ftok("client.c", rand() % 256);
    int qid = msgget(key, IPC_CREAT | IPC_PRIVATE | S_IRUSR | S_IWUSR);
    if (qid == -1) perror("creating the private FIFO");
    printf("key: %d\n", key);

    int msgsize = 100;
    size_t buffsize = sizeof(my_msgbuf) + msgsize;
    my_msgbuf* buffer = malloc(buffsize);
    buffer->type = 1;
    buffer->cmd = START;
    sprintf(buffer->args, "%d", key);
    if (msgsnd(srv_qid, buffer, buffsize, 0) == -1) perror("sending the message");
    msgrcv(qid, buffer, buffsize, 0, 0);
    char command [15];
    while (1) {
        printf("Input command: ");
        scanf("%[A-Z] %[0-9. ]", command, buffer->args);
        int cmd = command_to_int(command);
        if (cmd == -1) printf("%s is a wrong command. Type 'help'.\n", command);
        if (msgsnd(srv_qid, buffer, buffsize, 0)) perror("sending the command");
        msgrcv(qid, buffer, buffsize, 0, 0);
        printf("Result: %s\n", buffer->args);
    }
    free(buffer);
    exit (EXIT_SUCCESS);
}

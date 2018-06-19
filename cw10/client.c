#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include "colors.h"
#include "communicate.h"

#define err(msg) do { perror(msg); exit(EXIT_SUCCESS); } while (0);

enum method_t { 
    NET = 1, 
    UNIX = 2
};

void            try_add_client          (const int sockfd, const char* path);
void            print_usage             (const char* name);
const char*     read_name               (const char* arg);
enum method_t   read_connection_method  (const char* arg);
int             compute_task            (const task_t* task, task_t* result);
void            use_SIGINT              (int _);

int deregister = 0;

int main (const int argc, const char** argv)
{
    if (argc < 4) {
        print_usage(argv[0]);
        exit(EXIT_SUCCESS);
    }
    signal(SIGINT, use_SIGINT);

    const char* name = read_name(argv[1]); 
    const enum method_t connection_method = read_connection_method(argv[2]);
    const int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    task_t recv_task;
    task_t send_task;

    strcpy(send_task.from, name);
    cli_addr.sun_family = SOCK_STREAM;
    strcpy(cli_addr.sun_path, argv[3]);
    connect(sockfd, (struct sockaddr*) &cli_addr, clilen);

    while (1) {

        if (deregister) {
            printf(C_YELLOW "Interrupted." C_RESET " Ending...\n");
            strcpy(send_task.op, "end");
            send(sockfd, &send_task, sizeof(task_t), 0);
            exit(EXIT_SUCCESS);
        }

        recv(sockfd, &recv_task, sizeof(task_t), 0);
        if (strcmp(recv_task.op, "ping") == 0) {
            strcpy(send_task.op, "pong");
            send(sockfd, &send_task, sizeof(task_t), 0);
        } else {
            printf("Received" C_YELLOW "[%d]" C_RESET
                    ": %s %f %f\n", recv_task.id, recv_task.op, recv_task.arg1, recv_task.arg2);
            int status = compute_task(&recv_task, &send_task);
            if (status == -1) {
                printf(C_RED "Error" C_RESET
                        ": bad task format.\n");
            } else {
                printf("Computation " C_YELLOW "[%d]" C_RESET
                        ": %f\n", send_task.id, send_task.arg1);
                send(sockfd, &send_task, sizeof(task_t), 0);
            }
        }
    }

    exit(EXIT_SUCCESS);
}

int compute_task (const task_t* task, task_t* result)
{
    if (strcasecmp(task->op, "ADD") == 0) result->arg1 = task->arg1 + task->arg2;
    else if (strcasecmp(task->op, "SUB") == 0) result->arg1 = task->arg1 - task->arg2;
    else if (strcasecmp(task->op, "MUL") == 0) result->arg1 = task->arg1 * task->arg2; 
    else if (strcasecmp(task->op, "DIV") == 0) result->arg1 = task->arg1 / task->arg2;
    else return -1;
    strcpy(result->op, "EQ");
    result->arg2 = 0;
    result->id = task->id;
    return 0;
}

int connect_client (const int sockfd, const char* path)
{
    struct sockaddr_un cli_addr;
    cli_addr.sun_family = SOCK_STREAM;
    strcpy(cli_addr.sun_path, path);
    socklen_t clilen = (socklen_t) sizeof(cli_addr);
    if (connect(sockfd, (struct sockaddr*) &cli_addr, clilen) == -1) {
        return -1;
    }
    return 0;
}

void use_SIGINT (int _)
{
    deregister = 1;
}

// ----------------- CONSOLE INPUT HANDLING ----------------------------------------

void print_usage (const char* name)
{
    printf( C_YELLOW "Usage: " C_RESET 
            "%s <name> <connection_method> <server_address>\n", name);
}

const char* read_name (const char* arg) 
{
    if (strlen(arg) >= MAX_CLIENT_NAME) {
        printf(C_RED "Error" C_RESET 
                ": client name longer than %d\n", MAX_CLIENT_NAME);
        exit(EXIT_FAILURE);
    }
    return arg;
}

enum method_t read_connection_method (const char* arg) 
{
    if (strcasecmp(arg, "net") == 0) {
        return NET; 
    } else if (strcasecmp(arg, "unix") == 0) {
        return UNIX;
    } else {
        printf("<connection_method> is not [net | unix]\n");
        exit(EXIT_FAILURE);
    }
    return NET;
}

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

char g_socket_path[200];

void                print_usage             (const char* name);
void                clean_up                (void);
void                use_SIGINT              (int _);
int                 read_task               (task_t* task);
int                 serv_init               (const char* path);

int main (const int argc, const char** argv)
{
    if (argc < 3) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    atexit(clean_up);
    signal(SIGINT, use_SIGINT);

    const int port_number = atoi(argv[1]);
    const int sockfd = serv_init(argv[3]);
    struct sockaddr_un cli_addr;

    socklen_t clilen = (socklen_t) sizeof(cli_addr);
    printf("Waiting for clients...\n");
    int newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
    if (newsockfd < 0) {
        err("accept client");
    }
    printf("Connection accepted.\n");

    while (1) {
        task_t task;
        if (read_task(&task) == -1) continue;
        send(newsockfd, &task, sizeof(task_t), 0);
        recv(newsockfd, &task, sizeof(task_t), 0);
        printf("Result: %lf\n", task.arg1);
    }

    exit(EXIT_SUCCESS);
}

void print_usage (const char* name)
{
    printf(C_YELLOW "Usage:" C_RESET " %s <port_number> <socket_path>\n", name);
}

int serv_init (const char* path)
{
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = SOCK_STREAM;
    strcpy(g_socket_path, path);
    strcpy(serv_addr.sun_path, path); 

    if (sockfd == -1) {
        err("socket");
    }
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        err("bind");
    }
    if (listen(sockfd, 15) == -1) {
        err("listen");
    }

    return sockfd;
}

int read_task (task_t* task)
{
    char op [100];
    scanf("%s %lf %lf", op, &task->arg1, &task->arg2);
    if (strlen(op) > 3) {
        printf(C_RED "Error: " C_RESET 
                "operator %s is neither of [ADD|SUB|MUL|DIV]\n", op);
        return -1;
    }
    strcpy(task->op, op);
    // DEBUG
        printf("Task: %s, %f, %f", task->op, task->arg1, task->arg2);
    return 0;
}

void clean_up ()
{
    unlink(g_socket_path);
}

void use_SIGINT (int _)
{
    printf(C_YELLOW "Interrupted." C_RESET " Closing...\n");
    exit(EXIT_SUCCESS);
}

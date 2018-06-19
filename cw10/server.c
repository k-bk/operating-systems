#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include "colors.h"
#include "communicate.h"

#define err(msg) do { perror(msg); exit(EXIT_SUCCESS); } while (0);
#define MAX_CLIENTS 30
#define DEBUG 0

typedef struct thread_arg_t {
    int sockfd;
    int port_number;
} thread_arg_t;

typedef struct client_t {
    int fd;
    char name [MAX_CLIENT_NAME + 1];
    time_t active;
} client_t;

void            print_usage             (const char* name);
void            clean_up                (void);
void            use_SIGINT              (int _);
int             read_console_input      (task_t* task);
int             serv_init               (const char* path, const int port);
void*           serv_console_thread     (void* arg);
void*           serv_controller_thread  (void* arg);
void*           serv_ping_thread        (void* arg);
int             send_ping               (int sockfd);
void            print_task              (task_t* task);
void            remove_client           (int i);

char g_socket_path[200];
int g_task_count = 0;
int g_client_count = 0;
struct pollfd g_client_poll [MAX_CLIENTS];
client_t g_clients [MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int main (const int argc, const char** argv)
{
    if (argc < 3) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    atexit(clean_up);
    signal(SIGINT, use_SIGINT);

    thread_arg_t thread_arg;
    pthread_t controller_thread;
    pthread_t ping_thread;

    thread_arg.port_number = htobe16(atoi(argv[1]));
    thread_arg.sockfd = serv_init(argv[2], thread_arg.port_number);

    printf("Server initialized.\n");

    pthread_create(&controller_thread, NULL, serv_controller_thread, &thread_arg);
    pthread_create(&ping_thread, NULL, serv_ping_thread, NULL);

    int client_id = 0;
    while (1) {
        task_t task;
        memset(&task, 0, sizeof(task_t));
        read_console_input(&task);

        pthread_mutex_lock(&clients_mutex);
        if (g_client_count > 0) {
            send(g_clients[client_id].fd, &task, sizeof(task_t), 0);
            client_id = (client_id + 1) % g_client_count;
        } else {
            printf(C_YELLOW "Error: " C_RESET 
                    "no clients connected, try again later :(\n");
        }
        g_task_count++;
        pthread_mutex_unlock(&clients_mutex);
    }
    exit(EXIT_SUCCESS);
}

void print_usage (const char* name)
{
    printf(C_YELLOW "Usage:" C_RESET " %s <port_number> <socket_path>\n", name);
}

int serv_init (const char* path, const int port)
{
    int sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    int sockfd2 = socket(AF_INET6, SOCK_STREAM | SOCK_NONBLOCK, 0);

    struct sockaddr_un serv_addr;
    struct sockaddr_in6 serv_addr_net;
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(g_socket_path, path);
    strcpy(serv_addr.sun_path, path); 

    memset(&serv_addr_net, 0, sizeof(serv_addr_net));
    serv_addr_net.sin6_family = AF_INET6;
    serv_addr_net.sin6_port = port;
    serv_addr_net.sin6_addr = in6addr_any;


    if (sockfd == -1 || sockfd2 == -1) {
        err("socket");
    }
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        err("bind");
    }
    if (bind(sockfd2, (struct sockaddr*) &serv_addr_net, sizeof(serv_addr_net)) == -1) {
        err("bind net");
    }
    if (listen(sockfd, MAX_CLIENTS) == -1) {
        err("listen");
    }
    if (listen(sockfd2, MAX_CLIENTS) == -1) {
        err("listen net");
    }

    return sockfd;
}

void* serv_console_thread (void* arg)
{
    return NULL;
}

void* serv_controller_thread (void* arg)
{
    const thread_arg_t* thread_arg = arg;

    // client array initialization
    for (int i = 0; i < MAX_CLIENTS; i++) {
        g_client_poll[i].fd = -1;
        g_client_poll[i].events = POLLIN | POLLOUT;
        g_client_poll[i].revents = POLLIN | POLLOUT;
        g_clients[i].active = 0;
    }

    struct sockaddr_un cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    printf("Waiting for clients...\n");
    while (1) {

        // receive any message and act accordingly
        pthread_mutex_lock(&clients_mutex);
        if (g_client_count > 0) {
            int ready = poll(g_client_poll, g_client_count, 1000);
            if (ready > 0) {
                task_t recv_task;
                for (int i = 0; i < g_client_count; i++) {
                    if (g_client_poll[i].revents & POLLIN) {
                        recv(g_clients[i].fd, &recv_task, sizeof(task_t), 0);
                        print_task(&recv_task);
                        if (strcmp(recv_task.op, "end") == 0) {
                            remove_client(i);
                        } else {
                            g_clients[i].active = time(NULL);
                        }
                    }
                    if (g_client_poll[i].revents & POLLOUT) {
                        g_clients[i].active = time(NULL);
                    }
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        int newsockfd = accept(thread_arg->sockfd, (struct sockaddr*) &cli_addr, &clilen);
        if (newsockfd < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                err("accept client");
            }
        } else {
            printf("\n Connection accepted.\n");
            pthread_mutex_lock(&clients_mutex);
            g_client_poll[g_client_count].fd = newsockfd;
            g_clients[g_client_count].active = time(NULL);
            g_clients[g_client_count].fd = newsockfd;
            g_client_count++;
            pthread_mutex_unlock(&clients_mutex);
        }
    }
    return NULL;
}

void* serv_ping_thread (void* arg)
{
    task_t ping_task;
    strcpy(ping_task.op, "ping");
    strcpy(ping_task.from, "server");

    while (1) {
        time_t act_time = time(NULL);
        for (int i = 0; i < g_client_count; i++) {
            if (act_time - g_clients[i].active > 5) {
                printf("Client timeout");
                pthread_mutex_lock(&clients_mutex);
                remove_client(i);
                pthread_mutex_unlock(&clients_mutex);
            }
        }

        for (int i = 0; i < g_client_count; i++) {
            send(g_clients[i].fd, &ping_task, sizeof(task_t), 0);
            print_task(&ping_task);
        }

        sleep(1);
    }
    return NULL;
}

void remove_client (int i)
{
    g_client_count--;
    if (g_client_count > 0) {
        memcpy(&g_clients[i], &g_clients[g_client_count], sizeof(client_t));
        g_client_poll[i].fd = g_client_poll[g_client_count].fd;
        g_client_poll[i].revents = g_client_poll[g_client_count].revents;
    }
}

int read_console_input (task_t* task)
{
    char usr_input [100];
    char op [100];
    int tokens_read;

    printf(" > ");
    fgets(usr_input, sizeof(usr_input) - 1, stdin);
    tokens_read = sscanf(usr_input, "%s %lf %lf", op, &task->arg1, &task->arg2);

    if (tokens_read != 3) {
        printf(C_RED "Error: " C_RESET 
                "bad task format [operator] [arg] [arg]\n");
        return -1;
    }
    if (strlen(op) > 3) {
        printf(C_RED "Error: " C_RESET 
                "operator %s is neither of [ADD|SUB|MUL|DIV]\n", op);
        return -1;
    }

    strcpy(task->op, op);
    strcpy(task->from, "server");
    task->id = g_task_count;
    print_task(task);
    return 0;
}

void print_task (task_t* task)
{
    if (strcmp(task->op, "ping") == 0) {
        if (DEBUG) printf("Ping: %s\n", task->from);
    } else if (strcmp(task->op, "pong") == 0) {
        if (DEBUG) printf("Pong: %s\n", task->from);
    } else if (strcmp(task->op, "end") == 0) {
        printf("Client deregistered: %s\n", task->from);
    } else if (strcmp(task->from, "server") == 0) {
        printf("[%d] Sending: %s %f %f\n"
                , task->id, task->op, task->arg1, task->arg2);
    } else {
        printf("[%d] Received: %s %f from " C_YELLOW "%s" C_RESET "\n"
                , task->id, task->op, task->arg1, task->from);
    }
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

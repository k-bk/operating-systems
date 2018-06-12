#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include "colors.h"

#define err(msg) do { perror(msg); exit(EXIT_SUCCESS); } while (0);

void doprocessing (int sock);
void clean_up ();

char socket_path[200];

void clean_up ()
{
    unlink(socket_path);
}

void use_SIGINT (int _)
{
    printf(C_RED "Interrupted." C_RESET " Closing...\n");
    exit(EXIT_SUCCESS);
}

int main (const int argc, const char** argv)
{
    if (argc < 3) {
        printf(C_RED "Usage:" C_RESET " %s <port_number> <socket_path>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    atexit(clean_up);
    signal(SIGINT, use_SIGINT);

    const int port_number = atoi(argv[1]);
    const int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un serv_addr;
    struct sockaddr_un cli_addr;
    char* message;

    serv_addr.sun_family = SOCK_STREAM;
    strcpy(socket_path, argv[2]);
    strcpy(serv_addr.sun_path, socket_path); 

    if (sockfd == -1) {
        err("socket");
    }
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        err("bind");
    }
    if (listen(sockfd, 32) == -1) {
        err("listen");
    }

    while (1) {
        printf("Waiting for clients...\n");
        socklen_t clilen = (socklen_t) sizeof(cli_addr);
        int newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        if (newsockfd < 0) {
            close(sockfd);
            err("accept client");
        }
        printf("Connection accepted.\n");

        message = "Hello Client, I have got a connection with you.\n";
        write(newsockfd, message, strlen(message));
    }
    
    exit(EXIT_SUCCESS);
}

void doprocessing (int sock)
{
    int n;
    char buffer[256];
    bzero(buffer, 256);
    n = read(sock, buffer, 255);

    if (n < 0)
        err("read from socket");

    printf("Hello world: %s", buffer);
    n = write(sock, "I got your message", 18);

    if (n < 0) 
        err("write to socket");

}

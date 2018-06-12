#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define err(msg) do { perror(msg); exit(EXIT_SUCCESS); } while (0);

void doprocessing (int sock);

int main (const int argc, const char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <port_number> <socket_path>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    const int port_number = atoi(argv[1]);
    const char* socket_path = argv[2];
    const int sockfd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    int newsockfd;
    struct sockaddr_un serv_addr;
    struct sockaddr_un cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int pid;

    serv_addr.sun_family = SOCK_STREAM;
    strcpy(serv_addr.sun_path, socket_path); 

    if (sockfd == -1) {
        unlink(socket_path);
        err("socket");
    }
    if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        unlink(socket_path);
        err("bind");
    }
    if (listen(sockfd, 32) == -1) {
        unlink(socket_path);
        err("listen");
    }

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);

        if (newsockfd < 0)
            err("accept client");

        pid = fork();

        if (pid < 0)
            err("fork");

        if (pid == 0) {
            close(sockfd);
            doprocessing(newsockfd);
            exit(EXIT_SUCCESS);
        } else {
            close(newsockfd);
        }
    }
    
    unlink(socket_path);
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include "colors.h"

#define err(__msg) do { perror(__msg); exit(EXIT_SUCCESS); } while (0);

enum method_t { NET = 1, UNIX = 2};

char socket_path [200];

int main (const int argc, const char** argv)
{
    if (argc < 4) {
        printf( "Usage: %s <name> <connection_method> <server_address>\n"
                , argv[0]);
        exit(EXIT_SUCCESS);
    }

    const char* name = argv[1];
    const int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    enum method_t connection_method = NET;
    if (strcasecmp(argv[2], "net") == 0) {
        connection_method = NET;
    } else if (strcasecmp(argv[2], "unix") == 0) {
        connection_method = UNIX;
    } else {
        printf("<connection_method> is not [net | unix]\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_un cli_addr;

    cli_addr.sun_family = SOCK_STREAM;
    strcpy(socket_path, argv[2]);

    socklen_t clilen = (socklen_t) sizeof(cli_addr);
    if (connect(sockfd, (struct sockaddr*) &cli_addr, clilen) == -1) {
        err("connect");
    }

    exit(EXIT_SUCCESS);
}

// //////
// Karol Bak
// //////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


int main (int argc, char** argv) {
    FILE* stream;
    FILE* dateStream;
    char* path;
    int N;
    char date[100];

    srand(time(NULL));

    if (argc != 3) {
        printf("Usage slave <path-to-fifo> <num-of-msgs>\n");
        return EXIT_FAILURE;
    }
    path = argv[1];
    N = atoi(argv[2]);
    if((stream = fopen(path, "w")) == NULL)
        perror("Error while opening the file");
    printf("PID: %d\n", getpid());

    for (int i = 0; i < N; i++) {
        char message[100];
        dateStream = popen("date", "r");
        fread(date, sizeof(char), sizeof(date), dateStream);
        sprintf(message, "ID: %d\t %s", getpid(), date);
        printf("%s", message);
        fwrite(message, sizeof(char), sizeof(message), stream);
        fflush(stream);
        sleep(rand() % 5 + 2);
    }

    return EXIT_SUCCESS;
}

// //////
// Karol Bak
// //////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

FILE* stream;

void sig_INT(int signum) {
    signum++;
    fclose(stream);
    exit(EXIT_SUCCESS);
}

void run_master () {
    char buffer[100] = {0};
    while (1) {
        if (fread(buffer, 1, sizeof(buffer), stream))
            printf("%s", buffer);
    }
    fclose(stream);
}

int main (int argc, char** argv) {
    if (argc != 2) {
        printf("Usage master <path-to-FIFO>\n");
        return EXIT_FAILURE;
    }

    char* path = argv[1];
    mode_t mode = S_IRUSR | S_IWUSR;

    if(mkfifo(path, mode) == -1) {
        perror("Unable to make fifo at path");
    };
    if((stream = fopen(path, "r")) == NULL)
        perror("Error while opening the file");
    signal(SIGINT, sig_INT);
    run_master();
    return EXIT_SUCCESS;
}

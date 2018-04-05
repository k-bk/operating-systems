#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define BUFF_SIZE 100000
#define MAX_ARGS 50

char* pargv[MAX_ARGS];

void print_help (char* prog_name) {
    fprintf(stderr, "Usage: %s <file_name> [<time_limit>(sec) <mem_limit>(MB)]\n", prog_name);
}

char file_buffer[BUFF_SIZE];

int main (int argc, char** argv) {

    if (!(argc == 4 || argc == 2)) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    struct rlimit time_limit, mem_limit;
    struct rusage usage;
    if (argc == 4) {
        time_limit.rlim_max = atoi(argv[2]);
        mem_limit.rlim_max = atoi(argv[3]);
    }

    char* file_name = argv[1];
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("A problem occured when opening the file %s\n", file_name);
        return EXIT_FAILURE;
    }

    char* line = NULL;
    char line_copy[100];
    char* token = NULL;
    ssize_t read;
    size_t length = 0;
    pid_t child_pid, w;
    int status;

    while ((read = getline(&line, &length, file)) != -1) {
        strcpy(line_copy, line);
        int i = 0;
        while ((token = strsep(&line, " \n")) != NULL) {
            if (strlen(token) > 0) {
                pargv[i] = token;
                i++;
            }
        }

        child_pid = fork();
        if (child_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {
            printf("----------\nExec: %s", line_copy);
            execvp(pargv[0], pargv);
            if (argc == 4) {
                if (setrlimit(RLIMIT_CPU, &time_limit) == -1) {
                    printf("Error while setting the time limit\n");
                    exit(EXIT_FAILURE);
                }
                if (setrlimit(RLIMIT_DATA, &mem_limit) == -1) {
                    printf("Error while setting the memory limit\n");
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            w = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);
            if (getrusage(RUSAGE_CHILDREN, &usage) == 0) { 
                printf("maxrss:   %ld\n"
                       "ixrss:    %ld\n"
                       "idrss:    %ld\n"
                       "isrss:    %ld\n"
                       "minflt:   %ld\n"
                       "majflt:   %ld\n"
                       "nsignals: %ld\n"
                       , usage.ru_maxrss
                       , usage.ru_ixrss
                       , usage.ru_idrss
                       , usage.ru_minflt
                       , usage.ru_majflt
                       , usage.ru_nsignals);
            }
            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0) {
                    printf("Execution of %sended with status %d (error).\n", line_copy, WEXITSTATUS(status));
                    exit(EXIT_FAILURE);
                } else {
                    printf("exited, status = %d\n", WEXITSTATUS(status));
                }
            } else {
                printf("No exit status for %s.", line_copy);
            }
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

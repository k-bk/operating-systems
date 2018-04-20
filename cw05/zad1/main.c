//
// Karol Bak
//

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define BUFF_SIZE 100000
#define MAX_ARGS 50


void print_help (char* prog_name) {
    fprintf(stderr, "Usage: %s <file_name>\n", prog_name);
}

int print_signal_info (int status, char* line) {
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            printf("Execution of '%s' \nended with status %d (error).\n", line, WEXITSTATUS(status));
            return EXIT_FAILURE;
        }
    } else if (WIFSIGNALED(status)) {
        printf("Execution of '%s' \nkilled by signal %d.\n", line, WTERMSIG(status));
        return EXIT_FAILURE;
    } else if (WIFSTOPPED(status)) {
        printf("Execution of '%s' \nstopped by signal %d.\n", line, WSTOPSIG(status));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int execute_one_program (char* argv) {
    char* argv_copy = strdup(argv);
    char* token = NULL;
    char* pargv[MAX_ARGS];
    int i = 0;
    while ((token = strsep(&argv_copy, " ")) != NULL) {
        if (strlen(token) > 0) {
            pargv[i] = token;
            i++;
        }
    }
    return execvp(pargv[0], pargv);
}

int execute_one_line (char* line) {

    char* line_copy = strdup(line);
    char* program = NULL;
    pid_t child_pid;

    int pfd[2];
    int fd[2];

    int i = 0;
    while ((program = strsep(&line_copy, "|")) != NULL) {
        pfd[0] = fd[0];
        pfd[1] = fd[1];
        pipe(fd);
        child_pid = fork();
        if (child_pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        } else if (child_pid == 0) {
            close(fd[0]);
            close(pfd[1]);
            dup2(fd[1], STDOUT_FILENO);
            dup2(pfd[0], STDIN_FILENO);
            if (i == 0) close(pfd[0]);
            execute_one_program(program);
            exit(EXIT_SUCCESS);
        }
        close(pfd[0]);
        close(pfd[1]);
        close(fd[1]);
        i += 1;
    }
    char buf[100];
    read(fd[0], buf, sizeof(buf));
    printf("%s", buf);
    return EXIT_SUCCESS;
}

int main (int argc, char** argv) {

    if (argc != 2) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    char* file_name = argv[1];
    FILE* file = fopen(file_name, "r");
    char* file_buffer = (char*) calloc(BUFF_SIZE, sizeof(char));
    if (file == NULL) {
        printf("A problem occured when opening the file %s\n", file_name);
        return EXIT_FAILURE;
    }
    fread(file_buffer, 1, BUFF_SIZE, file);

    char* line = NULL;

    while ((line = strsep(&file_buffer, "\n")) != NULL) {
        if (strlen(line) > 1) {
            if (execute_one_line(line) == EXIT_FAILURE) break;
        }
    }

    free(file_buffer);
    fclose(file);
    return EXIT_SUCCESS;
}

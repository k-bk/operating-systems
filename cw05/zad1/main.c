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
    fprintf(stderr, "Usage: %s <file_name> [<time_limit>(sec) <mem_limit>(MB)]\n", prog_name);
}

void print_rusage (struct rusage* start, struct rusage* end) {
    time_t usr_delta_time = 
        (end->ru_utime.tv_sec - start->ru_utime.tv_sec) * 1000000 +
        end->ru_utime.tv_usec - start->ru_utime.tv_usec;
    time_t sys_delta_time = 
        (end->ru_stime.tv_sec - start->ru_stime.tv_sec) * 1000000 +
        end->ru_stime.tv_usec - start->ru_stime.tv_usec;
    printf("Usr time: %ld \n"
        "Sys time: %ld \n"
        "maxrss:   %f \n"
        , usr_delta_time
        , sys_delta_time
        , end->ru_maxrss / 1024.0);
}

int print_signal_info (int status, char* line) {
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            printf("Execution of '%s' \nended with status %d (error).\n"
                , line, WEXITSTATUS(status));
            return EXIT_FAILURE;
        }
    } else if (WIFSIGNALED(status)) {
        printf("Execution of '%s' \nkilled by signal %d.\n"
            , line, WTERMSIG(status));
        return EXIT_FAILURE;
    } else if (WIFSTOPPED(status)) {
        printf("Execution of '%s' \nstopped by signal %d.\n"
            , line, WSTOPSIG(status));
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

    struct rusage usage_start;
    struct rusage usage_end;

    char* line_copy = strdup(line);
    char* program = NULL;
    pid_t child_pid;
    int status;

    while ((program = strsep(&line_copy, "|")) != NULL) {
        if (strlen(program) > 0) {
            printf("----------\nExec: %s\n", program);
            child_pid = fork();
            if (child_pid == -1) {
                perror("fork");
                return EXIT_FAILURE;
            } else if (child_pid == 0) {
                execute_one_program(program);
                exit(EXIT_SUCCESS);
            }
            getrusage(RUSAGE_CHILDREN, &usage_start);
            while (wait(&status) != -1);
            getrusage(RUSAGE_CHILDREN, &usage_end);
            print_rusage(&usage_start, &usage_end);
        }
    }
    free(line_copy);

    if(print_signal_info(status, line) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main (int argc, char** argv) {

    int fd[2];
    pipe(fd);
    if (fork() == 0) { // dziecko
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execlp("echo", "echo","sshhoor\nriidiAlano\nrlsntl\n", NULL);
    } else if (fork() == 0) { // kolejne dziecko
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execlp("grep", "grep","Ala", NULL);
    } 

    /*if (argc != 2) {
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

    free(line);
    free(file_buffer);
    fclose(file);
    return EXIT_SUCCESS;
    */
}

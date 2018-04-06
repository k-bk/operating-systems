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

char* pargv[MAX_ARGS];

void print_help (char* prog_name) {
    fprintf(stderr, "Usage: %s <file_name> [<time_limit>(sec) <mem_limit>(MB)]\n", prog_name);
}

char file_buffer[BUFF_SIZE];

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
	   "inblock:  %ld \n"
	   "oublock:  %ld \n"
	   , usr_delta_time
	   , sys_delta_time
	   , end->ru_maxrss / 1024.0
	   , end->ru_inblock
	   , end->ru_oublock);
}

int main (int argc, char** argv) {

    if (!(argc == 4 || argc == 2)) {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    struct rlimit time_limit, mem_limit;
    struct rusage usage_start;
    struct rusage usage_end;
    if (argc == 4) {
        time_limit.rlim_cur = time_limit.rlim_max = atoi(argv[2]);
        mem_limit.rlim_cur = mem_limit.rlim_max = atoi(argv[3]) * 1024 * 1024;
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

	getrusage(RUSAGE_CHILDREN, &usage_start);
        child_pid = fork();
        if (child_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {
            if (argc == 4) {
                if (setrlimit(RLIMIT_CPU, &time_limit) == -1) {
                    printf("Error while setting the time limit\n");
                    exit(EXIT_FAILURE);
                }
                if (setrlimit(RLIMIT_AS, &mem_limit) == -1) {
                    printf("Error while setting the memory limit\n");
                    exit(EXIT_FAILURE);
                }
            }
            printf("----------\nExec: %s", line_copy);
            execvp(pargv[0], pargv);
        } else {
            w = waitpid(child_pid, &status, 0);

            if (w == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) != 0) {
                    printf("Execution of %sended with status %d (error).\n"
			    , line_copy, WEXITSTATUS(status));
                    exit(EXIT_FAILURE);
		}
            } else if (WIFSIGNALED(status)) {
		printf("Execution of %skilled by signal %d.\n"
			, line_copy, WTERMSIG(status));
		exit(EXIT_FAILURE);
            } else if (WIFSTOPPED(status)) {
		printf("Execution of %sstopped by signal %d.\n"
			, line_copy, WSTOPSIG(status));
		exit(EXIT_FAILURE);
	    }

            getrusage(RUSAGE_CHILDREN, &usage_end);
	    print_rusage(&usage_start, &usage_end);
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

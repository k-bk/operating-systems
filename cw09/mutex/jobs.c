#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define err(x) do { perror(x); exit(EXIT_FAILURE); } while(0)
#define BUFSIZE 10000000
#define FILTER_SIZE 100000

int show_details = 0;

enum smode_t { GT = 1, LT, EQ };
enum pmode_t { M_NORMAL, M_MINIMAL };

typedef struct config_t
{
    int P;
    int K;
    int N;
    char* file_name;
    int L;
    enum smode_t search_mode;
    enum pmode_t print_mode;
    int nk;
} config_t;

void clean_up () 
{
}

int read_config (config_t* config)
{
    const int bufsize = 2000;
    char* buffer = malloc(bufsize);
    const char* config_path = "config.ini";

    FILE* fconf = fopen(config_path, "r");
    if (fread(buffer, 1, bufsize, fconf) == -1) {
        fclose(fconf);
        return EXIT_FAILURE;
    }
    fclose(fconf);

    char* line;
    char* name;
    char* value;
    while ((line = strsep((char**) &buffer, "\n")) != NULL) {
        printf("line = %s\n", line);
        while (1) {
            name = strsep(&line, " \t");
            if (name == NULL) break;
            if (name[0] == '\0') continue;
            else if (name[0] == '#') {
                name = NULL;
                break;
            }
            else break;
        }
        printf("    name = %s\n", name);
        while (1) {
            value = strsep(&line, " \t");
            if (value == NULL) break;
            if (value[0] == '\0') continue;
            else if (value[0] == '#') {
                value = NULL;
                break;
            }
            else break;
        }
        printf("    value = %s\n", value);
        if (name && value) {
            if (value[0] == '#') continue;
            if (strcmp(name, "P") == 0) config->P = atoi(value);
            if (strcmp(name, "K") == 0) config->K = atoi(value);
            if (strcmp(name, "N") == 0) config->N = atoi(value);
            if (strcmp(name, "file_name") == 0) {
                int slen = strlen(value);
                config->file_name = malloc(slen);
                strcpy(config->file_name, value);
            }
            if (strcmp(name, "L") == 0) config->L = atoi(value);
            if (strcmp(name, "search_mode") == 0) {
                if (strcmp(value, "GT") == 0) {
                    config->search_mode = GT;
                } else if (strcmp(value, "LT") == 0) {
                    config->search_mode = LT;
                } else {
                    config->search_mode = EQ;
                }
            }
            if (strcmp(name, "print_mode") == 0) {
                if (strcmp(value, "normal") == 0) {
                    config->print_mode = M_NORMAL;
                } else {
                    config->print_mode = M_MINIMAL;
                }
            }
            if (strcmp(name, "nk") == 0) config->nk = atoi(value);
        }
    }
    free(buffer);
    return EXIT_SUCCESS;
}

int main (const int argc, const char **argv) 
{
    config_t config;
    read_config (&config);
    printf( "P = %d\n"
            "K = %d\n"
            "N = %d\n"
            "fname = %s\n"
            "L = %d\n"
            "smode = %d\n"
            "pmode = %d\n"
            "nk = %d\n"
            , config.P, config.K, config.N, config.file_name, config.L, config.search_mode, config.print_mode, config.nk);
    exit(EXIT_SUCCESS);
} 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define C_RED     "\x1b[31m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m"
#define C_BLUE    "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN    "\x1b[36m"
#define C_RESET   "\x1b[0m"

#define err(x) do { perror(x); exit(EXIT_FAILURE); } while(0)
#define BUFSIZE 10000000
#define FILTER_SIZE 100000

int show_details = 0;

enum smode_t { GT = 1, LT, EQ };
enum pmode_t { M_NORMAL, M_MINIMAL };

typedef struct config_t {
    int P;
    int K;
    int N;
    char* file_name;
    int L;
    enum smode_t search_mode;
    enum pmode_t print_mode;
    int nk;
} config_t;

typedef struct array_t {
    int N;
    char** buffer;
    int produce;
    int consume;
    int elems;
} array_t;

void clean_up () 
{
}

// ------------ ARRAYS -----------------------------------------

int array_create (array_t* array, int N) {
    array->buffer = (char**) malloc(N * sizeof(char*));
    array->produce = 0;
    array->consume = 0;
    array->elems = 0;
    array->N = N;
    return EXIT_SUCCESS;
}

int array_add (array_t* array, char* value) 
{
    array->buffer[array->produce] = strdup(value);
    array->produce++;
    array->produce %= array->N;
    array->elems++;
    return EXIT_SUCCESS;
}

int array_is_full (array_t array) 
{
    return array.elems == array.N;
}

char* array_consume (array_t* array) 
{
    if (array->elems <= 0) {
        return NULL;
    }

    char* value = strdup(array->buffer[array->consume]);
    free(array->buffer[array->consume]);
    array->buffer[array->consume] = NULL;
    array->consume++;
    array->consume %= array->N;
    array->elems--;
    return value;
}

void array_print (array_t array) 
{
    printf("---\n");
    for (int i = 0; i < array.N; i++) {
        if (array.buffer[i] != NULL) {
            printf(C_YELLOW "[%d]" C_RESET "\t%s\n", i, array.buffer[i]);
        } else if (i+1 < array.N && array.buffer[i+1] != NULL) {
            printf("\t...\n");
        }
    }
}

void array_delete (array_t* array)
{
    for (int i = 0; i < array->N; i++) {
        if (array->buffer[i] != NULL) {
            free(array->buffer[i]);
        }
    }
    free(array->buffer);
}

// ------------ CONFIG -----------------------------------------

int config_read (config_t* config)
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
    // TODO - refactoring: move deleting comments to separate function
    while ((line = strsep((char**) &buffer, "\n")) != NULL) {
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

int config_test (config_t config) 
{
    printf( "---\n"
            "config.ini\n"
            "P = %d K = %d N = %d\n"
            "fname = %s\n"
            "L = %d\n"
            "smode = %d pmode = %d\n"
            "nk = %d\n"
            , config.P, config.K, config.N, config.file_name
            , config.L, config.search_mode, config.print_mode, config.nk);
    return EXIT_SUCCESS;
}

void config_delete (config_t config) 
{
    free(config.file_name);
}

// ------------ MAIN -------------------------------------------

int main (const int argc, const char **argv) 
{
    config_t config;
    config_read(&config);
    config_test(config);
    
    array_t array;
    array_create(&array, config.N);
    array_add(&array, "super linia");
    array_add(&array, "kolejna linijka");

    config_delete(config);
    array_delete(&array);
    exit(EXIT_SUCCESS);
} 

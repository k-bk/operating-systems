#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

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
FILE* source;

enum smode_t { GT = 1, LT = -1, EQ = 0 };
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

typedef struct thread_arg_t {
    pthread_t thread;
    config_t* config;
    array_t* array;
} thread_arg_t;

void clean_up () 
{
}

int cmp_int (const int l1, const int l2) 
{
    if (l1 < l2) return -1;
    if (l1 > l2) return 1;
    return 0;
}

char* cmp_to_string (const int cmp) 
{
    if (cmp == EQ) return "EQ";
    if (cmp == LT) return "LT";
    return "GT";
}

char* read_line (FILE* fd)
{
    char line[500];
    fgets(line, 500, fd);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = 0;
        return strdup(line);
    } else {
        return NULL;
    }
}

// ------------ MUTEXES ----------------------------------------

pthread_mutex_t array_add_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t array_consume_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t array_length = PTHREAD_COND_INITIALIZER;

// ------------ ARRAYS -----------------------------------------

int array_create (array_t* array, int N)
{
    array->buffer = (char**) malloc(N * sizeof(char*));
    array->produce = 0;
    array->consume = 0;
    array->elems = 0;
    array->N = N;
    return EXIT_SUCCESS;
}

int array_is_full (const array_t* array) 
{
    return array->elems >= array->N;
}

int array_is_empty (const array_t* array) 
{
    return array->elems <= 0;
}

int array_add (array_t* array, char* value) 
{
    pthread_mutex_lock(&array_add_mutex);
    while (array_is_full(array)) {
        pthread_cond_wait(&array_length, &array_add_mutex);
    }
    array->buffer[array->produce] = strdup(value);
    array->produce++;
    array->produce %= array->N;
    array->elems++;
    pthread_cond_broadcast(&array_length);
    pthread_mutex_unlock(&array_add_mutex);
    return EXIT_SUCCESS;
}

char* array_consume (array_t* array) 
{
    pthread_mutex_lock(&array_consume_mutex);
    while (array_is_empty(array)) {
        pthread_cond_wait(&array_length, &array_consume_mutex);
    }
    char* value = strdup(array->buffer[array->consume]);
    free(array->buffer[array->consume]);
    array->buffer[array->consume] = NULL;
    array->consume++;
    array->consume %= array->N;
    array->elems--;
    pthread_cond_broadcast(&array_length);
    pthread_mutex_unlock(&array_consume_mutex);
    return value;
}

void array_print (const array_t* array) 
{
    printf("---\n");
    for (int i = 0; i < array->N; i++) {
        if (array->buffer[i] != NULL) {
            printf(C_YELLOW "[%d]" C_RESET "\t%s\n", i, array->buffer[i]);
        } else if (i+1 < array->N && array->buffer[i+1] != NULL) {
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
    const char* config_path = "config.ini";

    FILE* fconf = fopen(config_path, "r");
    char* line;
    char* name;
    char* value;
    // TODO - refactoring: move deleting comments to separate function
    while ((line = read_line(fconf)) != NULL) {
        printf("line: %s\n", line);
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
                config->file_name = strdup(value);
            }
            if (strcmp(name, "L") == 0) config->L = atoi(value);
            if (strcmp(name, "search_mode") == 0) {
                if (strcasecmp(value, "GT") == 0) {
                    config->search_mode = GT;
                } else if (strcasecmp(value, "LT") == 0) {
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
        //free(line);
    }
    fclose(fconf);
    return EXIT_SUCCESS;
}

int config_test (const config_t config) 
{
    printf( "---\n"
            "config.ini\n"
            "P = %d K = %d N = %d\n"
            "fname = %s\n"
            "L = %d\n"
            "smode = %s pmode = %d\n"
            "nk = %d\n"
            , config.P, config.K, config.N, config.file_name
            , config.L, cmp_to_string(config.search_mode), config.print_mode
            , config.nk);
    return EXIT_SUCCESS;
}

void config_delete (config_t config) 
{
    free(config.file_name);
}

// ------------ CONSUMENT --------------------------------------

void* consumer (void* varg)
{
    time_t t_act;
    char* string;
    const time_t t_start = time(NULL);
    const thread_arg_t* arg = varg;
    config_t* config = arg->config;
    array_t* array = arg->array;
    do { 
        string = array_consume(array);
        int len = strlen(string);
        if (config->search_mode == cmp_int(len, config->L)) {
            printf(C_YELLOW "%lu" C_RESET "\t%s\n", arg->thread, string);
        }
        time(&t_act);
    } while (t_act - t_start < config->nk);
    return NULL;
}

// ------------ PRODUCER ---------------------------------------
//
void* producer (void* varg)
{
    time_t t_act;
    const time_t t_start = time(NULL);
    const thread_arg_t* arg = varg;
    config_t* config = arg->config;
    array_t* array = arg->array;
    char* line;
    do { 
        line = read_line(source);
        array_add(array, line); 
        free(line);
        time(&t_act);
    } while (t_act - t_start < config->nk);
    return NULL;
}

// ------------ MAIN -------------------------------------------

int main (const int argc, const char **argv) 
{
    config_t config;
    config_read(&config);
    config_test(config);
    
    array_t array;
    array_create(&array, config.N);
    if ((source = fopen(config.file_name, "r")) == NULL) perror("fopen");

    thread_arg_t* threads = malloc(sizeof(thread_arg_t) * (config.P + config.K));

    for (int i = 0; i < config.P; i++) {
        threads[i].array = &array;
        threads[i].config = &config;
        pthread_create(&threads[i].thread, NULL, producer, &threads[i]);
    }
    for (int i = config.P; i < config.P + config.K; i++) {
        threads[i].array = &array;
        threads[i].config = &config;
        pthread_create(&threads[i].thread, NULL, consumer, &threads[i]);
    }

    for (int i = 0; i < config.P + config.K; i++) {
        pthread_join(threads[i].thread, NULL);
    }
    config_delete(config);
    array_delete(&array);
    exit(EXIT_SUCCESS);
} 

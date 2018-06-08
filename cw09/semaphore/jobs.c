#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "arrays.h"
#include "colors.h"
#include "cfgparser.h"

#define err(x) do { perror(x); exit(EXIT_FAILURE); } while(0)

int producer_waiting_for_exit = 0; 
int consuments_exited = 0;
FILE* source;

typedef struct thread_arg_t {
    pthread_t thread;
    config_t* config;
    array_t* array;
} thread_arg_t;

int cmp_int (const int l1, const int l2) 
{
    if (l1 < l2) return -1;
    if (l1 > l2) return 1;
    return 0;
}

char* read_line (FILE* fd)
{
    char line[1000];
    fgets(line, 1000, fd);
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

// ------------ CONSUMENT --------------------------------------

void* consumer (void* varg)
{
    char* string;
    const thread_arg_t* arg = varg;
    config_t* config = arg->config;
    array_t* array = arg->array;
    do { 
        pthread_mutex_lock(&array_consume_mutex);
        while (array_is_empty(array)) {
            if (producer_waiting_for_exit) {
                consuments_exited++;
                pthread_mutex_unlock(&array_consume_mutex);
                goto cons_exit;
            }
            pthread_cond_wait(&array_length, &array_consume_mutex);
        }
        string = array_consume(array);
        pthread_mutex_unlock(&array_consume_mutex);
        pthread_cond_signal(&array_length);
        int len = strlen(string);
        if (config->search_mode == cmp_int(len, config->L)) {
            printf(C_YELLOW "%lu" C_RESET "\t%s\n", arg->thread, string);
        }
    } while (1);

cons_exit:

    return NULL;
}

// ------------ PRODUCER ---------------------------------------

void* producer (void* varg)
{
    time_t t_act;
    const time_t t_start = time(NULL);
    const thread_arg_t* arg = varg;
    config_t* config = arg->config;
    array_t* array = arg->array;

    char* line;
    while (1) { 
        line = read_line(source);
        if (line == NULL) break;
        pthread_mutex_lock(&array_add_mutex);
        while (array_is_full(array)) {
            pthread_cond_wait(&array_length, &array_add_mutex);
        }
        array_add(array, line); 
        pthread_mutex_unlock(&array_add_mutex);
        pthread_cond_signal(&array_length);
        free(line);
        time(&t_act);
        if (config->nk > 0 && t_act - t_start < config->nk) break;
    }

    if (!producer_waiting_for_exit) {
        producer_waiting_for_exit = 1;
        while (consuments_exited < config->K) {
            pthread_cond_broadcast(&array_length);
        }
    }

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
    if ((source = fopen(config.file_name, "r")) == NULL) err("fopen");

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
    config_delete(&config);
    array_delete(&array);
    exit(EXIT_SUCCESS);
} 

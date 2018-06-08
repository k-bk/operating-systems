#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arrays.h"
#include "colors.h"

// ------------ ARRAYS -----------------------------------------

int array_create (array_t* array, const int N)
{
    array->buffer = (char**) malloc(N * sizeof(char*));
    for (int i = 0; i < N; i++) array->buffer[i] = NULL;
    array->produce = 0;
    array->consume = 0;
    array->N = N;
    return EXIT_SUCCESS;
}

int array_is_full (const array_t* array) 
{
    return array->buffer[array->produce] != NULL; 
}

int array_is_empty (const array_t* array) 
{
    return array->buffer[array->consume] == NULL; 
}

int array_add (array_t* array, const char* value) 
{
    array->buffer[array->produce] = strdup(value);
    array->produce++;
    array->produce %= array->N;
    return EXIT_SUCCESS;
}

char* array_consume (array_t* array) 
{
    char* value = strdup(array->buffer[array->consume]);
    free(array->buffer[array->consume]);
    array->buffer[array->consume] = NULL;
    array->consume++;
    array->consume %= array->N;
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


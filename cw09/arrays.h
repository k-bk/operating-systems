#ifndef ARRAYS_H
#define ARRAYS_H

typedef struct array_t {
    int N;
    char** buffer;
    int produce;
    int consume;
    int elems;
} array_t;

int array_create (array_t* array, const int N);
int array_is_full (const array_t* array);
int array_is_empty (const array_t* array);
int array_add (array_t* array, const char* value);
char* array_consume (array_t* array);
void array_print (const array_t* array);
void array_delete (array_t* array);

#endif

#ifndef ARRAY_MANAGER_H
#define ARRAY_MANAGER_H

#define ST_MEM_COUNT 10000
#define ST_MEM_SIZE 1000

typedef struct Array {
    int block_size;
    int block_count;
    int is_static;
    char** blocks;
} Array;

char static_memory[ST_MEM_COUNT][ST_MEM_SIZE];

extern void create_array (Array* array, int block_count, int block_size, int is_static);

extern void delete_array (Array* array);

extern void create_block (Array* array, int index, char* block_contents);
    
extern void delete_block (Array* array, int index);

extern char* search_element (Array* array, int index);

extern void print_array (Array* array);

#endif //ARRAY_MANAGER_H

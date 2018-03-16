#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "array_manager.h"

void create_array (Array* array, int block_count, int block_size, int is_static) {

    assert(block_count > 0 && block_size > 0);
    assert(block_count < ST_MEM_COUNT && block_size < ST_MEM_SIZE);

    array->block_size = block_size;
    array->block_count = block_count;
    array->is_static = is_static; 

    if (is_static)
	array->blocks = (char**) static_memory;
    else 
	array->blocks = (char**) calloc (block_count, sizeof(char*));
}

void delete_array (Array* array) {
    if (array->is_static) {
	for (int i = 0; i < array->block_count; i++) {
	    for (int j = 0; j < array->block_size; j++) {
		static_memory[i][j] = 0;
	    }
	}
    } else {
	free (array->blocks);
    }
}

void delete_block (Array* array, int index) {
    assert(index >= 0 && index < array->block_count);

    if (array->is_static) {
	for (int i = 0; i < array->block_size; i++) {
	    static_memory[index][i] = 0;
	}
    } else {
	array->blocks[index] = NULL;
    }
}

void create_block (Array* array, int index, char* block_contents) {
    assert(index >= 0);

    if (array->is_static) {
	assert(index < ST_MEM_COUNT);
	for (int i = 0; i < array->block_size; i++) {
	    static_memory[index][i] = block_contents[i];
	}
    } else {
	int i = 0;
	while (i < array->block_count && array->blocks[i] != NULL) i++;
	if (i >= array->block_count) {
	    array->block_count = 2 * i;
	    array->blocks = (char**) realloc(array->blocks, 2 * i * sizeof(char*));
	}
	array->blocks[i] = (char*) calloc(array->block_size, sizeof(char));
	for (int j = 0; j < array->block_size; j++) {
	    array->blocks[i][j] = block_contents[j];
	}
    }
}

int sum_of_block (Array* array, int index) {
    assert(index >= 0 && index < array->block_count);

    int sum = 0;
    char* block;
    if (array->is_static)
	block = static_memory[index];
    else
	block = array->blocks[index];

    if (block != NULL)
	for (int i = 0; i < array->block_size; i++) sum += block[i];
    return sum;
}

char* search_element (Array* array, int index) {
    assert(index >= 0 && index < array->block_count);
    if (array->is_static) 
	printf("looking for: %s\n", static_memory[index]);
    else
	printf("looking for: %s\n", array->blocks[index]);

    int value = sum_of_block (array, index);
    int nearest_index = -1;
    int nearest_distance = 10000;
    for (int i = 0; i < array->block_count; i++) {
	int tmp_distance = abs (sum_of_block (array, i) - value);
	if (tmp_distance < nearest_distance && i != index) {
	    nearest_distance = tmp_distance; 
	    nearest_index = i; 
	}
    }
    if (array->is_static) 
	return static_memory[nearest_index];
    return array->blocks[nearest_index];
}

void print_array (Array* array) {
    for (int i = 0; i < array->block_count; i++) {
	for (int j = 0; j < array->block_size; j++) {
	    if (array->is_static) {
		printf("%c", static_memory[i][j]);
	    } else {
		if (array->blocks[i] != NULL) {
		    printf("%c", array->blocks[i][j]);
		} else {
		    printf("%s", "NULL"); 
		    break;
		}
	    }
	}
	printf("\n");
    }
    printf("\n");
}

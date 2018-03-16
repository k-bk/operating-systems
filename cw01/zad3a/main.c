#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include "array_manager.h"

#ifdef DYNAMIC

#include <dlfcn.h>

void* handle;

#endif

const char* program_name;

//////////// TIME

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start);
}



char* random_contents (char* block, int size) {
    for (int i = 0; i < size; i++) {
	block[i] = (char) (rand() % 10 + 48);
//	block[i] = (char) (rand() % 26 + 65);
    }
    block[size] = '\0';
    return block;
}

void print_usage (FILE* stream, int exit_code) {
    fprintf (stream, "Usage: %s output_filename is_static options\n", program_name);
    fprintf (stream, 
	"   create_table size block \n"
	"   search_element index \n"
	"   remove number \n"
	"   add number \n"
	"   remove_and_add number \n");
    exit (exit_code);
}


int main (int argc, char* argv[]) {

#ifdef DYNAMIC
    handle = dlopen("./array_manager.so", RTLD_LAZY);
    if (!handle) {
	printf("Error while loading a dynamic library.");
    }

    void (*create_array)(Array*, int, int, int) = dlsym (handle, "create_array");
    void (*delete_array)(Array*) = dlsym (handle, "delete_array");
    void (*create_block)(Array*, int, char*) = dlsym (handle, "create_block");
    void (*delete_block)(Array* array, int) = dlsym (handle, "delete_block");
    char* (*search_element)(Array*, int) = dlsym (handle, "search_element");
    void (*print_array)(Array*) = dlsym (handle, "print_array");

#endif

    time_t t;
    srand((unsigned) time(&t));

    program_name = argv[0];
    if (argc < 2)
	print_usage(stdout, 0);
    const char* output_filename = argv[1];

    FILE* file;
    file = fopen(output_filename, "a");

    fprintf(file, "------------------------------------------\n\n");

    const int is_static = atoi(argv[2]);

    int argument = 3; 
    Array* test = (Array*) malloc (sizeof(Array));

    clock_t start, end;
    struct tms* tms_start = (struct tms*) malloc(sizeof(struct tms));
    struct tms* tms_end = (struct tms*) malloc(sizeof(struct tms));

    while (argument < argc) {
	char* option = argv[argument];

	if (strcmp (option, "create_table") == 0) {
	    argument++;
	    int size = atoi (argv[argument]);
	    argument++;
	    int block = atoi (argv[argument]);

	    start = times(tms_start);
	    create_array (test, size, block, is_static);
	    for (int i = 0; i < size; i++) {
		char* contents = (char*) malloc (size * sizeof(char));
		contents = random_contents(contents, size);
		create_block (test, i, contents);
	    }
	    end = times(tms_end);

	} else if (strcmp (option, "search_element") == 0) {
	    argument++;
	    int index = atoi (argv[argument]);
	    start = times(tms_start);
	    char* result = search_element (test, index);
	    end = times(tms_end);
	    printf ("found: %s \n", result);
	} else if (strcmp (option, "remove") == 0) {
	    argument++;
	    int number = atoi (argv[argument]);

	    start = times(tms_start);
	    for (int i = 0; i < number; i++) {
		delete_block (test, test->block_count - 1);
	    }
	    end = times(tms_end);

	} else if (strcmp (option, "add") == 0) {
	    argument++;
	    int number = atoi (argv[argument]);

	    start = times(tms_start);
	    for (int i = 0; i < number; i++) {
		char* contents = (char*) malloc (test->block_size * sizeof(char));
		contents = random_contents(contents, test->block_size);
		create_block (test, test->block_count, contents);
	    }
	    end = times(tms_end);

	} else if (strcmp (option, "remove_and_add") == 0) {
	    argument++;
	    int number = atoi (argv[argument]);

	    start = times(tms_start);
	    for (int i = 1; i <= number; i++) {
		delete_block (test, test->block_count - i);
		char* contents = (char*) malloc (test->block_size * sizeof(char));
		contents = random_contents(contents, test->block_size);
		create_block (test, test->block_count - i, contents);
	    }
	    end = times(tms_end);
	} 

	fprintf (file, "option: %s allocation: %d\n", option, is_static);
	fprintf (file, "total time: %f\n", calculate_time (start, end));
	fprintf (file, "cpu_time: %f\n", 
		calculate_time (tms_start->tms_utime, tms_end->tms_utime));
	fprintf (file, "cpu_time: %f\n", 
		calculate_time (tms_start->tms_stime, tms_end->tms_stime));
	argument++;
    }
    fclose(file);

#ifdef DYNAMIC
    if (dlclose(handle) != 0) {
	printf("Error while closing the library.");
    }
#endif
    return 0;
}

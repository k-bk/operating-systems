#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include "array_manager.h"

const char* program_name;

//////////// TIME

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC; 
}

/////////////////

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

    time_t t;
    srand((unsigned) time(&t));

    program_name = argv[0];
    if (argc < 2)
	print_usage(stdout, 0);
    const char* output_filename = argv[1];
    const int is_static = atoi(argv[2]);

    int argument = 3; 
    Array* test = (Array*) malloc (sizeof(Array));

    clock_t start, end;

    struct tms start1, end1;

    while (argument < argc) {
	char* option = argv[argument];

	if (strcmp (option, "create_table") == 0) {
	    argument++;
	    int size = atoi (argv[argument]);
	    argument++;
	    int block = atoi (argv[argument]);

	    start = clock();
	    times (&start1);
	    
	    create_array (test, size, block, is_static);
	    for (int i = 0; i < size; i++) {
		char* contents = (char*) malloc (size * sizeof(char));
		contents = random_contents(contents, size);
		create_block (test, i, contents);
	    }
	    end = clock();
	    times (&end1);

	} else if (strcmp (option, "search_element") == 0) {
	    argument++;
	    int index = atoi (argv[argument]);
	    start = clock();
	    char* result = search_element (test, index);
	    end = clock();
	    printf ("found: %s \n", result);
	} else if (strcmp (option, "remove") == 0) {
	    argument++;
	    int number = atoi (argv[argument]);

	    start = clock();
	    times (&start1);

	    for (int i = 0; i < number; i++) {
		delete_block (test, test->block_count - 1);
	    }
	    end = clock();
	    times (&end1);

	} else if (strcmp (option, "add") == 0) {
	    argument++;
	    int number = atoi (argv[argument]);

	    start = clock();
	    times (&start1);
	    for (int i = 0; i < number; i++) {
		char* contents = (char*) malloc (test->block_size * sizeof(char));
		contents = random_contents(contents, test->block_size);
		create_block (test, test->block_count, contents);
	    }
	    end = clock();
	    times (&end1);

	} else if (strcmp (option, "remove_and_add") == 0) {
	    argument++;
	    int number = atoi (argv[argument]);

	    start = clock();
	    times (&start1);
	    for (int i = 1; i <= number; i++) {
		delete_block (test, test->block_count - i);
		char* contents = (char*) malloc (test->block_size * sizeof(char));
		contents = random_contents(contents, test->block_size);
		create_block (test, test->block_count - i, contents);
	    }
	    end = clock();
	    times (&end1);
	} 

	printf ("option: %s \nreal:  %f\n"
		"             utime: %f\n"
		"             stime: %f\n"
		, option
		, calculate_time (start, end)
		, calculate_time (start1.tms_utime, end1.tms_utime)
		, calculate_time (start1.tms_stime, end1.tms_utime)
		);
	argument++;
    }
    return 0;
}

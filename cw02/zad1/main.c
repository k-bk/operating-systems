#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_iocmp.h"
#include "sys_iocmp.h"
#include <time.h>
#include <sys/times.h>

const char* progName;

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC; 
}

void showHelp () {
    printf ("Use %s <function> [args]\n"
	    "generate [name] [count] [regsize] [lib/sys]\n"
	    "sort     [name] [count] [regsize] [lib/sys]\n"
	    "copy     [name1] [name2] [count] [regsize] [lib/sys]\n"
	    , progName);
}

int main (int argc, char* argv[]) {

    srand (time (NULL));
    progName = argv[0];

    clock_t start, end;

    struct tms start1, end1;

    start = clock();
    times (&start1);
    if (argc > 1) {
	if (strcmp (argv[1], "generate") == 0 && argc > 5) {
	    char *name = argv[2];
	    int count = atoi (argv[3]);
	    int size = atoi (argv[4]);
	    RecordFile recordFile = newRecordFile (name, size, count);

	    char *mode = argv[5];
	    printf ("generate %d %d, %s\n", count, size, mode);
	    if (strcmp (mode, "sys") == 0) {
		sys_generate (recordFile);
	    } else {
		lib_generate (recordFile);
	    }
	} else if (strcmp (argv[1], "sort") == 0 && argc > 5) {
	    char *name = argv[2];
	    int count = atoi (argv[3]);
	    int size = atoi (argv[4]);
	    RecordFile recordFile = newRecordFile (name, size, count);

	    char *mode = argv[5];
	    printf ("sort %d %d, %s\n", count, size, mode);
	    if (strcmp (mode, "sys") == 0) {
		sys_sort (recordFile);
	    } else {
		lib_sort (recordFile);
	    }
	} else if (strcmp (argv[1], "copy") == 0 && argc > 6) {
	    char *name1 = argv[2];
	    char *name2 = argv[3];
	    int linesToCopy = atoi (argv[4]);
	    int bufferSize = atoi (argv[5]);

	    char *mode = argv[6];
	    printf ("copy %d %d, %s\n", linesToCopy, bufferSize, mode);
	    if (strcmp (mode, "sys") == 0) {
		sys_copy (name1, name2, linesToCopy, bufferSize);
	    } else {
		lib_copy (name1, name2, linesToCopy, bufferSize);
	    }
	} else {
	    showHelp();
	}
    } else {
	showHelp();
    }
    end = clock();
    times(&end1);

    printf ("real:  %f\n"
	    "utime: %f\n"
	    "stime: %f\n\n"
	    , calculate_time (start, end)
	    , calculate_time (start1.tms_utime, end1.tms_utime)
	    , calculate_time (start1.tms_stime, end1.tms_utime)
	    );

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_iocmp.h"
#include <time.h>

const char* progName;

void showHelp () {
    printf ("Use %s function [args]\n", progName);
}

int main (int argc, char* argv[]) {

    srand (time (NULL));
    progName = argv[0];

    if (argc > 1) {
	if (strcmp (argv[1], "generate") == 0 && argc > 5) {
	    char *name = argv[2];
	    int count = atoi (argv[3]);
	    int size = atoi (argv[4]);
	    RecordFile recordFile = newRecordFile (name, count, size);

	    char *mode = argv[5];
	    if (strcmp (mode, "sys") == 0) {
		//sys_generate (recordFile);
	    } else {
		lib_generate (recordFile);
	    }
	} else if (strcmp (argv[1], "sort") == 0 && argc > 5) {
	    char *name = argv[2];
	    int count = atoi (argv[3]);
	    int size = atoi (argv[4]);
	    RecordFile recordFile = newRecordFile (name, count, size);

	    char *mode = argv[5];
	    if (strcmp (mode, "sys") == 0) {
		//sys_sort (recordFile);
	    } else {
		lib_sort (recordFile);
	    }
	} else if (strcmp (argv[1], "copy") == 0 && argc > 6) {
	    char *name1 = argv[2];
	    char *name2 = argv[3];
	    int linesToCopy = atoi (argv[4]);
	    int bufferSize = atoi (argv[5]);

	    char *mode = argv[6];
	    if (strcmp (mode, "sys") == 0) {
		//sys_copy (recordFile1, recordFile2, size);
	    } else {
		lib_copy (name1, name2, linesToCopy, bufferSize);
	    }
	} else {
	    showHelp();
	}
    } else {
	showHelp();
    }

    return 0;
}

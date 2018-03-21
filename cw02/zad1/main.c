#include <stdio.h>
#include <stdlib.h>
//#include "iocmp.h"
#include <time.h>

const char* progName;

void showHelp () {
    printf ("Use %s function [args]", progName);
}

int main (int argc, char* argv[]) {

    srand (time (NULL));
    progName = argv[0];

    int opt = 1;
    if (strcmp (argv[opt], "generate") == 0) {

    } else if (strcmp (argv[opt], "sort") == 0) {

    } else if (strcmp (argv[opt], "copy") == 0) {

    } else {
	showHelp();
    }

    return 0;
}

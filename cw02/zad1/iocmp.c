#include <stdlib.h>
#include <stdio.h>
#include "iocmp.h"

int generate (const char *name, int recordSize, int recordCount) {
    FILE *randomFile = fopen ("/dev/random", "r");
    FILE *resultFile = fopen (name, "w");
    if (randomFile == NULL || resultFile == NULL) {
	perror ("Error while opening the file.");
	return -1;
    }
    
    /*
    CONTENT GENERATION THAT USES /dev/random

    char randInt;
    for (int i = 0; i < recordCount; i++) {
    if (fread (&randInt, sizeof(randInt), 1, randomFile) != sizeof(randInt)) {
    for (int i = 0; i < size

    for (int i = 0; i < size; i++) {
	// add newline or change random int into printable character
	if ((i + 1) % (recordSize + 1) == 0)
	    randChar[i] = '\n';
	else
	    randChar[i] = (unsigned int) randChar[i] % 50 + 48;
    }
    randChar[size - 1] = '\0';
    fclose (randomFile);
    */

    int size = recordCount * (recordSize + 1);
    char out[recordCount][recordSize + 1];
    for (int i = 0; i < recordCount; i++) {
	for (int j = 0; j <= recordSize; j++) {
	    out[i][j] = rand() % 50 + 48;
	}
	out[i][recordSize] = '\n';
    }
    fwrite (out, 1, size, resultFile);
    fclose (randomFile);
    fclose (resultFile);

    // testing the compare funciton
    compare (name, recordSize, 3, 5);
    compare (name, recordSize, 5, 3);
    compare (name, recordSize, 1, 8);
    return 0;
}

int compare (const char *name, int recordSize, int i, int j) {
    FILE *file = fopen (name, "r");
    char iVal[1];
    char jVal[1];
    fseek (file, i * (recordSize + 1), 0);
    fread (iVal, sizeof(char), 1, file);
    fseek (file, j * (recordSize + 1), 0);
    fread (jVal, sizeof(char), 1, file);
    printf ("compare: i=%d j=%d \n iVal=%d %c jVal=%d %c cmp=%d \n", i, j, iVal[0], iVal[0], jVal[0], jVal[0], iVal[0] > jVal[0]);
    fclose (file);
    return iVal[0] > jVal[0];
}

int sort (const char *name, int recordSize) {

    return 0;
}

int copy (const char *name1, const char *name2, int recordSize) {
    return 0;
}

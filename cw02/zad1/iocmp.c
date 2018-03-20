#include <stdlib.h>
#include <stdio.h>
#include "iocmp.h"

char outprint[1000000] = {0};

RecordFile newRecordFile (const char *name, int size, int count) {
   RecordFile file; 
   file.name = name;
   file.size = size;
   file.count = count;
   return file;
}

int generate (RecordFile recordFile) {
    FILE *randomFile = fopen ("/dev/random", "r");
    FILE *resultFile = fopen (recordFile.name, "w");
    if (randomFile == NULL || resultFile == NULL) {
	perror ("Error while opening the file.");
	return -1;
    }
    
    /*
    CONTENT GENERATION THAT USES /dev/random

    char randInt;
    for (int i = 0; i < file.count; i++) {
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

    int size = recordFile.count * (recordFile.size + 1);
    char out[recordFile.count][recordFile.size + 1];
    for (int i = 0; i < recordFile.count; i++) {
	for (int j = 0; j <= recordFile.size; j++) {
	    out[i][j] = rand() % 26 + 65;
	}
	out[i][recordFile.size] = '\n';
    }
    fwrite (out, 1, size, resultFile);
    fclose (randomFile);
    fclose (resultFile);

    // testing the compare funciton
    sort (recordFile);
    print(recordFile);
    return 0;
}

void print (RecordFile recordFile) {
    FILE *file = fopen (recordFile.name, "r");
    if (file) {
	fseek (file, 0, 0);
	fread (outprint, 1, 100000, file);
	printf ("%s", outprint);
    }
    fclose (file);
}

int compare (RecordFile recordFile, int i, int j) {
    FILE *file = fopen (recordFile.name, "r");
    char iVal[1];
    char jVal[1];
    fseek (file, i * (recordFile.size + 1), 0);
    fread (iVal, sizeof(char), 1, file);
    fseek (file, j * (recordFile.size + 1), 0);
    fread (jVal, sizeof(char), 1, file);
    //printf ("compare: i=%d j=%d \n iVal=%d %c jVal=%d %c cmp=%d \n", i, j, iVal[0], iVal[0], jVal[0], jVal[0], jVal[0] - iVal[0]);
    fclose (file);
    return jVal[0] - iVal[0];
}

void swap (RecordFile recordFile, int i, int j) {
    FILE *file = fopen (recordFile.name, "r+");
    char iRecord[recordFile.size];
    char jRecord[recordFile.size];
    fseek (file, i * (recordFile.size + 1), 0);
    fread (iRecord, sizeof(char), recordFile.size, file);
    fseek (file, j * (recordFile.size + 1), 0);
    fread (jRecord, sizeof(char), recordFile.size, file);
    fseek (file, i * (recordFile.size + 1), 0);
    fwrite (jRecord, recordFile.size, 1, file);
    fseek (file, j * (recordFile.size + 1), 0);
    fwrite (iRecord, recordFile.size, 1, file);
    fclose (file);
}

int sort (RecordFile recordFile) {
    for (int i = 1; i < recordFile.count; i++) {
	int j = i - 1;
	while (j >= 0 && compare (recordFile, j, j+1) < 0) {
	    swap (recordFile, j, j+1);
	    j -= 1;
	}
    }
    return 0;
}

int copy (RecordFile file1, RecordFile file2) {
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "iocmp.h"

int sys = 0;

RecordFile newRecordFile (const char *name, int size, int count) {
   RecordFile file; 
   file.name = name;
   file.size = size;
   file.count = count;
   return file;
}

char outprint[1000000] = {0};

int generate (RecordFile recordFile) {

	int randomFile = open ("/dev/random", O_RDONLY);
	int resultFile = open (recordFile.name, O_WRONLY | O_CREAT);
	assert (randomFile >= 0 && resultFile >= 0);

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

	if (sys) {
		write (resultFile, out, size);
		close (randomFile);
		close (resultFile);
	} else {
		fwrite (out, 1, size, resultFile);
		fclose (randomFile);
		fclose (resultFile);
	}
    return 0;
}

void print (RecordFile recordFile) {
    FILE *file = fopen (recordFile.name, "r");
    if (file) {
		fread (outprint, 1, 100000, file);
		printf ("%s", outprint);
    }
    fclose (file);
}

int compare (RecordFile recordFile, int i, int j) {
    char iVal[1];
    char jVal[1];
	if (sys) {
		int file = open (recordFile.name, O_RDWR);
		lseek (file, i * (recordFile.size + 1), SEEK_SET);
		read (file, iRecord, sizeof(char) * recordFile.size);
		lseek (file, j * (recordFile.size + 1), SEEK_SET);
		read (file, jRecord, sizeof(char) * recordFile.size);
		lseek (file, i * (recordFile.size + 1), SEEK_SET);
		write (file, jRecord, recordFile.size);
		lseek (file, j * (recordFile.size + 1), SEEK_SET);
		write (file, iRecord, recordFile.size);
		close (file);
	} else {
		FILE *file = fopen (recordFile.name, "r");
		fseek (file, i * (recordFile.size + 1), 0);
		fread (iVal, sizeof(char), 1, file);
		fseek (file, j * (recordFile.size + 1), 0);
		fread (jVal, sizeof(char), 1, file);
		fclose (file);
	}
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

int copy (RecordFile recordFile1, RecordFile recordFile2, int linesToCopy) {
    FILE *file1 = fopen (recordFile1.name, "r");
    FILE *file2 = fopen (recordFile2.name, "a");
    assert (file1 && file2);
    assert (recordFile1.size == recordFile2.size);

    unsigned long bufferSize = recordFile1.size + 1;
    char buffer[recordFile1.size + 1];
    
    for (int i = 0; i < linesToCopy; i++) {
	fread (buffer, bufferSize, 1, file1);
	fwrite (buffer, bufferSize + 1, 1, file2);
    }	    
    fclose (file1);
    fclose (file2);
    return 0;
}

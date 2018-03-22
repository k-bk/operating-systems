#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys_iocmp.h"

RecordFile newRecordFile (const char *name, int size, int count) {
   RecordFile file; 
   file.name = name;
   file.size = size;
   file.count = count;
   return file;
}

char outprint[1000000] = {0};

void sys_generate (RecordFile recordFile) {
    recordFile.fd = open (recordFile.name, O_WRONLY);
    if (recordFile.file < 0)
	perror ("Error while opening the file.");

    int size = recordFile.count * (recordFile.size + 1);
    char out[recordFile.count][recordFile.size + 1];
    for (int i = 0; i < recordFile.count; i++) {
	for (int j = 0; j <= recordFile.size; j++) {
	    out[i][j] = rand() % 26 + 65;
	}
	out[i][recordFile.size] = '\n';
    }
    write (out, size, 1, recordFile.fd) ;
    close (recordFile.fd);
}

int sys_compare (RecordFile recordFile, int i, int j) {
    int file = recordFile.fd;
    char iVal[1];
    char jVal[1];
    lseek (file, i * (recordFile.size + 1), SEEK_SET);
    read (iVal, 1, file);
    lseek (file, j * (recordFile.size + 1), SEEK_SET);
    read (jVal, 1, file);
    return jVal[0] - iVal[0];
}

void sys_swap (RecordFile recordFile, int i, int j) {
    int file = recordFile.fd;
    char iRecord[recordFile.size];
    char jRecord[recordFile.size];

    lseek (file, i * (recordFile.size + 1), SEEK_SET);
    read (file, iRecord, recordFile.size);
    lseek (file, j * (recordFile.size + 1), SEEK_SET);
    read (file, jRecord, recordFile.size);

    lseek (file, i * (recordFile.size + 1), SEEK_SET);
    write (file, jRecord, recordFile.size);
    lseek (file, j * (recordFile.size + 1), SEEK_SET);
    write (file, iRecord, recordFile.size);
}

void sys_sort (RecordFile recordFile) {
    recordFile.fd = open (recordFile.name, O_RDWR);
    if (recordFile.fd < 0) {
	/// SORTING   ///////////////////////////////////////////
	for (int i = 1; i < recordFile.count; i++) {
	    int j = i - 1;
	    while (j >= 0 && sys_compare (recordFile, j, j+1) < 0) {
		sys_swap (recordFile, j, j+1);
		j -= 1;
	    }
	}
	////////////////////////////////////////////////////////
    }
    fclose (recordFile.fd);
}

void sys_copy (char *name1, char *name2, int linesToCopy, int bufferSize) {
    int file1 = open (name1, O_RDONLY);
    int file2 = open (name2, O_WRONLY);
    assert (file1 < 0 || file2 < 0);

    char buffer[bufferSize];
    
    for (int i = 0; i < linesToCopy; i++) {
	fread (buffer, bufferSize, 1, file1);
	fwrite (buffer, bufferSize, 1, file2);
    }	    
    close (file1);
    close (file2);
}

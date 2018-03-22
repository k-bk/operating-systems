#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys_iocmp.h"

mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP;

void sys_generate (RecordFile recordFile) {
    recordFile.fd = open (recordFile.name, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (recordFile.fd < 0)
	printf ("File not opened! File descriptor = %d\n"
		"Something's bad happening.\n", recordFile.fd);

    int size = recordFile.count * (recordFile.size + 1);
    char out[recordFile.count][recordFile.size + 1];
    for (int i = 0; i < recordFile.count; i++) {
	for (int j = 0; j <= recordFile.size; j++) {
	    out[i][j] = rand() % 26 + 65;
	}
	out[i][recordFile.size] = '\n';
    }
    write (recordFile.fd, out, size);
    close (recordFile.fd);
}

int sys_compare (RecordFile recordFile, int i, int j) {
    char iVal[1];
    char jVal[1];
    lseek (recordFile.fd, i * (recordFile.size + 1), SEEK_SET);
    read (recordFile.fd, iVal, 1);
    lseek (recordFile.fd, j * (recordFile.size + 1), SEEK_SET);
    read (recordFile.fd, jVal, 1);
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
    recordFile.fd = open (recordFile.name, O_RDWR, mode);
    if (recordFile.fd < 0)
	printf ("File not opened! File descriptor = %d\n"
		"Something's bad happening.\n", recordFile.fd);

    if (recordFile.fd > 0) {
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
    close (recordFile.fd);
}

void sys_copy (char *name1, char *name2, int linesToCopy, int bufferSize) {
    int file1 = open (name1, O_RDONLY, mode);
    int file2 = open (name2, O_APPEND | O_CREAT | O_WRONLY, mode);
    assert (file1 >= 0 && file2 >= 0);

    char buffer[bufferSize];
    
    for (int i = 0; i < linesToCopy; i++) {
	read (file1, buffer, bufferSize);
	write (file2, buffer, bufferSize);
    }	    
    close (file1);
    close (file2);
}

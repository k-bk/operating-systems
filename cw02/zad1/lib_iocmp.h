#ifndef LIB_IOCMP_H
#define LIB_IOCMP_H
#include <stdio.h>

typedef struct RecordFile {
    const char *name;
    int size;
    int count;
    FILE *file;
    int *fd;
} RecordFile;

extern RecordFile newRecordFile (const char *name, int size, int count);
extern void lib_generate (RecordFile recordFile);
extern void lib_sort (RecordFile recordFile);
extern void lib_copy (char *name1, char *name2, int linesToCopy, int bufferSize);
int lib_compare (RecordFile recordFile, int i, int j);
void lib_swap (RecordFile recordFile, int i, int j);
void lib_print (RecordFile recordFile);

#endif //LIB_IOCMP_H


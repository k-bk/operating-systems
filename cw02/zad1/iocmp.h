#ifndef IOCMP_H
#define IOCMP_H

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
extern void lib_copy (RecordFile recordFile1, RecordFile recordFile2, int linesToCopy);
int lib_compare (RecordFile recordFile, int i, int j);
void lib_swap (RecordFile recordFile, int i, int j);
void lib_print (RecordFile recordFile);

#endif //IOCMP_H


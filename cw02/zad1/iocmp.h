#ifndef IOCMP_H
#define IOCMP_H

typedef struct RecordFile {
    const char *name;
    int size;
    int count;
} RecordFile;

extern RecordFile newRecordFile (const char *name, int size, int count);
extern int generate (RecordFile recordFile);
extern int sort (RecordFile recordFile);
extern int copy (RecordFile recordFile1, RecordFile recordFile2, int linesToCopy);
int compare (RecordFile recordFile, int i, int j);
void swap (RecordFile recordFile, int i, int j);
void print (RecordFile recordFile);

#endif //IOCMP_H


#ifndef SYS_IOCMP_H
#define SYS_IOCMP_H
#include "lib_iocmp.h"

extern RecordFile newRecordFile (const char *name, int size, int count);
extern void sys_generate (RecordFile recordFile);
extern void sys_sort (RecordFile recordFile);
extern void sys_copy (char *name1, char *name2, int linesToCopy, int bufferSize);
int sys_compare (RecordFile recordFile, int i, int j);
void sys_swap (RecordFile recordFile, int i, int j);
void sys_print (RecordFile recordFile);

#endif //SYS_IOCMP_H


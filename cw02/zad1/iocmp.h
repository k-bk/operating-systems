#ifndef IOCMP_H
#define IOCMP_H

extern int generate (const char *name, int recordSize, int recordCount);
extern int sort (const char *name, int recordSize);
extern int copy (const char *name1, const char *name2, int recordSize);
extern int compare (const char *name, int recordSize, int i, int j);

#endif //IOCMP_H


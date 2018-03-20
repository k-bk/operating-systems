#include <stdio.h>
#include <stdlib.h>
#include "iocmp.h"
#include <time.h>

int main () {
    srand (time (NULL));
    const char *name = "nicefile.txt";
    const char *name2 = "better.txt";
    RecordFile recordFile = newRecordFile (name, 8, 10);
    RecordFile recordFile2 = newRecordFile (name2, 8, 5);
    generate (recordFile);
    sort (recordFile);
    generate (recordFile2);
    sort (recordFile);
    print (recordFile2);
    printf ("\n");
    copy (recordFile, recordFile2, 4);
    print (recordFile2);
    return 0;
}

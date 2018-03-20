#include <stdio.h>
#include <stdlib.h>
#include "iocmp.h"
#include <time.h>

int main () {
    srand (time (NULL));
    const char *name = "nicefile.txt";
    RecordFile recordFile = newRecordFile (name, 8, 10);
    generate (recordFile);
    return 0;
}

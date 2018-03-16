#include <stdlib.h>
#include "iocmp.h"

int generate (char *name, int recordSize) {
    FILE *file = fopen(name, w);
    if (file) {
	for (int i = 0; i < recordSize; i++) {
	}
    }
    return 0;
}

int sort (char *name, int recordSize) {

    return 0;
}

int copy (char *name1, char *name2, int recordSize) {

    return 0;
}

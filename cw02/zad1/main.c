#include <stdio.h>
#include <stdlib.h>
#include "iocmp.h"
#include <time.h>

int main () {
    srand (time (NULL));
    const char *name = "nicefile.txt";
    generate (name, 8, 10);
    return 0;
}

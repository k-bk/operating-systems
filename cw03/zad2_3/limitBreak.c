#include <stdlib.h>
#include <stdio.h>

int MAX_DEPTH = 15;

int loop (int level) {
    if (level > MAX_DEPTH) return EXIT_FAILURE;
    if (level % 512 == 0)
	printf("%.1f kB\n", level / 1024.0);
    char table[1024];
    for (int i = 0; i < 1024; i++) table[i] = 0;
    table[1]++;
    loop(level + 1);
    return EXIT_SUCCESS;
}

int calculate () {
    int result = 0;
    for (int i = 0; i < MAX_DEPTH; i++) {
	for (int j = 0; j < MAX_DEPTH; j++) {
	    result += i + j;
	}
    }
    printf("result: %d\n", result);
    return result;
}

int main (int argc, char** argv) {
    printf("Hello in limit break application.\n");
    if (argc > 1) {
	MAX_DEPTH = atoi(argv[1]);
    }
    if (argc > 2) { 
	printf("You have chosen to break time limit.\n");
	calculate();
    } else {
	printf("You have chosen to break memory limit.\n");
	loop(0);
    }
    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define err(x) do { perror(x); exit(EXIT_FAILURE); } while(0)
#define BUFSIZE 2000

void clean_up () 
{
}

void str_split (char* buffer, char** a_split, int* n)
{
    a_split = malloc(BUFSIZE * sizeof(char*));
    char* token;
    int i = 0;
    while((token = strsep(&buffer, "\n")) != NULL) {
        a_split[i] = token;
        i++;
    }
    a_split = realloc(a_split, i * sizeof(char*));
    *n = i;
}

int main (const int argc, const char **argv) 
{
    if (argc < 5) {
        printf("Usage: %s <threads> <file_in> <filter> <file_out>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    //int threads = atoi(argv[1]);
    const char* name_in = argv[2];
    //const char* name_filter = argv[3];
    //const char* name_out = argv[4];

    FILE* file_in = fopen(name_in, "r");
    if (file_in == NULL) err("fopen");
    char* buffer = malloc(BUFSIZE);
    size_t rd = fread(buffer, 1, BUFSIZE, file_in);
    if (rd == 0) err("fread");

    char** table = NULL;
    int len;
    str_split(buffer, table, &len);
    for (int i = 0; i < len; i++) {
        printf("%s\n", table[i]);
    }

    free(buffer);
    exit(EXIT_SUCCESS);
} 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define err(x) do { perror(x); exit(EXIT_FAILURE); } while(0)
#define BUFSIZE 1000

typedef struct header {
    int width;
    int height;
    int max_gray;
}header;

void clean_up () 
{
}

char** str_split (char** buffer, int* n, const char* delim)
{
    char** a_split = malloc(BUFSIZE * sizeof(char*));
    char* token;
    int i = 0;
    while((token = strsep(buffer, delim)) != NULL) {
        if (strcmp(token, "") != 0) {
            a_split[i] = token;
            i++;
        }
    }
    a_split = realloc(a_split, i * sizeof(char*));
    *n = i;
    return a_split;
}

int is_comment (const char* line) 
{
    while (*line == ' ') line++;
    return *line == '#';
}

int read_header (const char** buffer, header* head)
{
    int i = 0;
    while (is_comment(buffer[i])) i++;
    if (strcmp(buffer[i], "P2") != 0) return -1;
    i++;
    do {i++;} while (is_comment(buffer[i]));
    head->width = atoi(buffer[i]);
    do {i++;} while (is_comment(buffer[i]));
    head->height = atoi(buffer[i]);
    do {i++;} while (is_comment(buffer[i]));
    head->max_gray = atoi(buffer[i]);
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

    int num_of_lines;
    char** tokens = str_split(&buffer, &num_of_lines, "\n");
    header file_header;
    read_header(tokens, &file_header);
    free(tokens);
    free(buffer);
    exit(EXIT_SUCCESS);
} 

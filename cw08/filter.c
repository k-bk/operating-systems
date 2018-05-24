#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define err(x) do { perror(x); exit(EXIT_FAILURE); } while(0)
#define BUFSIZE 1000

typedef struct header {
    int width;
    int height;
    int max_gray;
}header;

void clean_up ();
char** str_split (char** buffer, int* n, const char* delim);
void remove_comments (char* buffer, int bufsize);
int read_header (char** buffer, header* head);

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

void remove_comments (char* buffer, const int bufsize) 
{
    int i = 0;
    while (i < bufsize && buffer[i] != 0) {
        if (buffer[i] == '#') {
            while (buffer[i] != '\n' && buffer[i] != 0) {
                buffer[i] = ' ';
                i++;
            }
        }
        if (buffer[i] == '\n') buffer[i] = ' ';
        if (buffer[i] == '\r') buffer[i] = ' ';
        i++;
    }
}

int read_header (char** buffer, header* head)
{
    if (strncmp(buffer[0], "P2", 2) != 0) {
        errno = EBADMSG;
        return -1;
    }
    head->width = atoi(buffer[1]);
    head->height = atoi(buffer[2]);
    head->max_gray = atoi(buffer[3]);
    return 0;
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
    remove_comments(buffer, rd);

    int num_of_tokens;
    char** tokens = str_split(&buffer, &num_of_tokens, " ");
    header file_header;
    if (read_header(tokens, &file_header) == -1) err("read_header");
    free(tokens);
    free(buffer);
    exit(EXIT_SUCCESS);
} 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/times.h>

#define err(x) do { perror(x); exit(EXIT_FAILURE); } while(0)
#define BUFSIZE 10000000
#define FILTER_SIZE 100000

int threads = 0;
int mx_in [BUFSIZE];
int mx_out [BUFSIZE];
double mx_filter [FILTER_SIZE];

typedef struct header {
    int width;
    int height;
    int max_gray;
} header;

typedef struct wrapped {
    int thread;
    int c;
    header head;
    int rbegin;
    int rend;
} wrapped;

void clean_up ();
char** str_split (char** buffer, int* n, const char* delim);
void remove_comments (char* buffer, const int bufsize);
int read_header (char** tokens, header* head);
int read_matrix (char** tokens, int n, int* matrix);

void clean_up () 
{
}

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / CLOCKS_PER_SEC;
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

int read_header (char** tokens, header* head)
{
    if (strncmp(tokens[0], "P2", 2) != 0) {
        errno = EBADMSG;
        return -1;
    }
    head->width = atoi(tokens[1]);
    head->height = atoi(tokens[2]);
    head->max_gray = atoi(tokens[3]);
    return 0;
}

int read_matrix (char** tokens, int n, int* matrix)
{
    for (int i = 0; i < n; i++) {
        if ((matrix[i] = atoi(tokens[i])) == -1) return -1;
    }
    return 0;
}

int save_matrix (const char* name_out, int* matrix, header head) {
    FILE* file_out = fopen(name_out, "w+");
    if (file_out == NULL) 
        err("fopen");
    fprintf(file_out, 
            "P2\n"
            "#%s\n"
            "%d %d\n"
            "%d\n"
            , name_out, head.width, head.height, 255);
    for (int i = 0; i < head.height * head.width; i++) {
        fprintf(file_out, "%3d", matrix[i]);
        if ((i+1) % 17 == 0) fprintf(file_out, "\n");
        else fprintf(file_out, " ");
    }
    return fclose(file_out);
}

int out_of_matrix (int val, int min, int max) 
{
    if (val < min) return -val;
    if (val > max) return 2*max - val;
    return 0;
}

int clamp (int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

int filter_cell (int x, int y, int* matrix, double* filter, int c, header head)
{
    double sum = 0;
    for (int i = 0; i < c; i++) {
        for (int j = 0; j < c; j++) {
            int a = x - floor(c/2) + i;
            int b = y - floor(c/2) + j;
            if (!out_of_matrix(a, 0, head.width - 1) && 
                    !out_of_matrix(b, 0, head.height - 1))
                sum += (double) matrix[a + b * head.width] * filter[i + j * c];
        }
    }
    int val = (int) (sum + 0.5);
    return clamp(val, 0, head.max_gray);
}

void apply_filter (int c, header head) {
    for (int x = 0; x < head.width; x++) {
        for (int y = 0; y < head.height; y++) {
            mx_out[x + y * head.width] = 
                filter_cell(x, y, mx_in, mx_filter, c, head);
        }
    }
}

void* apply_filter_threads (void* wr_args) {
    wrapped* args = (wrapped*) wr_args;
    for (int x = args->rbegin; x < args->rend; x++) {
        for (int y = 0; y < args->head.height; y++) {
            mx_out[x + y * args->head.width] = 
                filter_cell(x, y, mx_in, mx_filter, args->c, args->head);
        }
    }
    return NULL;
}

void normalize (double* array, int n) 
{
    double sum = 0;
    for (int i = 0; i < n; i++) sum += array[i];
    for (int i = 0; i < n; i++) array[i] /= sum;
}

int sqrt_int (int n) 
{
    int i;
    for (i = 1; i*i <= n; i++);
    return i - 1;
}

void read_filter (const char* name, int* c) {
    FILE* file_in = fopen(name, "r");
    if (file_in == NULL) 
        err("fopen");
    char* buffer = (char*) malloc(BUFSIZE);
    size_t rd = fread(buffer, 1, BUFSIZE, file_in);
    fclose(file_in);
    if (rd == 0) 
        err("fread");
    int num_of_tokens;
    char** tokens = str_split(&buffer, &num_of_tokens, " ");
    for (int i = 0; i < num_of_tokens; i++) {
        mx_filter[i] = atof(tokens[i]);
    }
    free(buffer);
    free(tokens);
    normalize(mx_filter, num_of_tokens);
    *c = sqrt_int(num_of_tokens);
}

int main (const int argc, const char **argv) 
{
    if (argc < 5) {
        printf("Usage: %s <threads> <file_in> <filter> <file_out>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    threads = atoi(argv[1]);
    const char* name_in = argv[2];
    const char* name_filter = argv[3];
    const char* name_out = argv[4];

    FILE* file_in = fopen(name_in, "r");
    if (file_in == NULL) 
        err("fopen");

    char* buffer = malloc(BUFSIZE);
    size_t rd = fread(buffer, 1, BUFSIZE, file_in);
    fclose(file_in);
    if (rd == 0) 
        err("fread");
    remove_comments(buffer, rd);

    int num_of_tokens;
    char** tokens = str_split(&buffer, &num_of_tokens, " ");
    header fhead;
    if (read_header(tokens, &fhead) == -1) 
        err("read_header");
    if (read_matrix(&tokens[4], fhead.width * fhead.height, mx_in) == -1) 
        err("read_matrix");

    int c;
    read_filter(name_filter, &c);

    clock_t clk_start, clk_end;
    struct tms start1, end1;
    // THREAD things
    pthread_t* a_threads = malloc(threads * sizeof(pthread_t));
    wrapped* tinfo = malloc(threads * sizeof(wrapped));
    clk_start = clock();
    times (&start1);
    for (int i = 0; i < threads; i++) {
        tinfo[i].c = c;
        tinfo[i].head = fhead;
        tinfo[i].thread = i;
        int part = (fhead.width - 1) / threads + 1;
        tinfo[i].rbegin = part * i;
        tinfo[i].rend = tinfo[i].rbegin + part;
        if (
                pthread_create(&(a_threads[i]), 
                NULL, 
                apply_filter_threads, 
                &tinfo[i]
        ) != 0)
            err("pthread_create");
    }

    for (int i = 0; i < threads; i++) {
        if (pthread_join(a_threads[i], NULL) != 0) 
            err("pthread_join");
    }
    clk_end = clock();
    times (&end1);

    if (save_matrix(name_out, mx_out, fhead) == -1) 
        err("save_matrix");

    printf ("threads: %d c: %d\n"
        "real:  %f\t"
		"utime: %f\t"
		"stime: %f\n"
		, threads
        , c
		, calculate_time (clk_start, clk_end)
		, calculate_time (start1.tms_utime, end1.tms_utime)
		, calculate_time (start1.tms_stime, end1.tms_utime)
    );

    free(a_threads);
    free(tinfo);
    free(tokens);
    free(buffer);
    exit(EXIT_SUCCESS);
} 

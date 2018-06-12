#ifndef CFGPARSER_H
#define CFGPARSER_H

enum smode_t { M_GT = 1, M_LT = -1, M_EQ = 0 };
enum pmode_t { M_NORMAL, M_MINIMAL };

typedef struct config_t {
    int P;
    int K;
    int N;
    char* file_name;
    int L;
    enum smode_t search_mode;
    enum pmode_t print_mode;
    int nk;
} config_t;

int config_test (const config_t config);
int config_read (config_t* config);
void config_delete (config_t* config);

#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cfgparser.h"

// ------------ CONFIG -----------------------------------------

char* cmp_to_string (const int cmp) 
{
    if (cmp == M_EQ) return "EQ";
    if (cmp == M_LT) return "LT";
    return "GT";
}

char* config_read_line (FILE* fd)
{
    char line[500];
    fgets(line, 500, fd);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = 0;
        return strdup(line);
    } else {
        return NULL;
    }
}

int config_read (config_t* config)
{
    const char* config_path = "config.ini";

    FILE* fconf = fopen(config_path, "r");
    char* line;
    char* name;
    char* value;
    // TODO - refactoring: move deleting comments to separate function
    while ((line = config_read_line(fconf)) != NULL) {
        printf("line: %s\n", line);
        while (1) {
            name = strsep(&line, " \t");
            if (name == NULL) break;
            if (name[0] == '\0') continue;
            else if (name[0] == '#') {
                name = NULL;
                break;
            }
            else break;
        }
        while (1) {
            value = strsep(&line, " \t");
            if (value == NULL) break;
            if (value[0] == '\0') continue;
            else if (value[0] == '#') {
                value = NULL;
                break;
            }
            else break;
        }
        if (name && value) {
            if (value[0] == '#') continue;
            if (strcmp(name, "P") == 0) config->P = atoi(value);
            if (strcmp(name, "K") == 0) config->K = atoi(value);
            if (strcmp(name, "N") == 0) config->N = atoi(value);
            if (strcmp(name, "file_name") == 0) {
                config->file_name = strdup(value);
            }
            if (strcmp(name, "L") == 0) config->L = atoi(value);
            if (strcmp(name, "search_mode") == 0) {
                if (strcasecmp(value, "GT") == 0) {
                    config->search_mode = M_GT;
                } else if (strcasecmp(value, "LT") == 0) {
                    config->search_mode = M_LT;
                } else {
                    config->search_mode = M_EQ;
                }
            }
            if (strcmp(name, "print_mode") == 0) {
                if (strcmp(value, "normal") == 0) {
                    config->print_mode = M_NORMAL;
                } else {
                    config->print_mode = M_MINIMAL;
                }
            }
            if (strcmp(name, "nk") == 0) config->nk = atoi(value);
        }
    }
    fclose(fconf);
    return EXIT_SUCCESS;
}

int config_test (const config_t config) 
{
    printf( "---\n"
            "config.ini\n"
            "P = %d K = %d N = %d\n"
            "fname = %s\n"
            "L = %d\n"
            "smode = %s pmode = %d\n"
            "nk = %d\n"
            , config.P, config.K, config.N, config.file_name
            , config.L, cmp_to_string(config.search_mode), config.print_mode
            , config.nk);
    return EXIT_SUCCESS;
}

void config_delete (config_t* config) 
{
    free(config->file_name);
}

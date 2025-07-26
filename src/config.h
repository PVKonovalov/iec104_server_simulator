#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#define NUM_COLUMNS 5

enum value_types {
    AI, // analogue input
    DI, // digital input
    AI_TS // AI with timestamp
};

typedef struct columns_s {
    int ioa;
    float val;
    float dev;
    char type[15];
    char type_gi[15];
} columns_s;


typedef struct config_t {
    int len;
    int *ioas;
    float *vals;
    float *devs;
    int *types;
    int *types_gi;
} config_t;

config_t *get_config(const std::string &configFilename);

#endif

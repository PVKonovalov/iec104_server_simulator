#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include "config.h"

#include <filesystem>

#include "cs104_slave.h"
#include "spdlog/spdlog.h"

static int type_str_to_enum(char *type_str) {
    if (strcmp(type_str, "M_SP_NA_1") == 0) {
        return M_SP_NA_1;
    }
    if (strcmp(type_str, "M_DP_NA_1") == 0) {
        return M_DP_NA_1;
    }
    if (strcmp(type_str, "M_ME_NA_1") == 0) {
        return M_ME_NA_1;
    }
    if (strcmp(type_str, "M_ME_NB_1") == 0) {
        return M_ME_NB_1;
    }
    if (strcmp(type_str, "M_ME_NC_1") == 0) {
        return M_ME_NC_1;
    }
    if (strcmp(type_str, "M_IT_NA_1") == 0) {
        return M_IT_NA_1;
    }
    if (strcmp(type_str, "M_ME_ND_1") == 0) {
        return M_ME_ND_1;
    }
    if (strcmp(type_str, "M_SP_TB_1") == 0) {
        return M_SP_TB_1;
    }
    if (strcmp(type_str, "M_DP_TB_1") == 0) {
        return M_DP_TB_1;
    }
    if (strcmp(type_str, "M_ME_TE_1") == 0) {
        return M_ME_TE_1;
    }
    if (strcmp(type_str, "M_ME_TF_1") == 0) {
        return M_ME_TF_1;
    }
    if (strcmp(type_str, "M_IT_TB_1") == 0) {
        return M_IT_TB_1;
    }
    if (strcmp(type_str, "M_ME_TD_1") == 0) {
        return M_ME_TD_1;
    }
    spdlog::warn("Skipping unrecognised type: {}", type_str);
    return -1;
}


config_t *get_config(const std::string &configFilename) {
    FILE *config_fd = nullptr;
    config_fd = fopen(configFilename.c_str(), "r");
    if (config_fd == nullptr) {
        spdlog::error("Failed to open config file {}", std::string(configFilename));
        exit(1);
    }

    // Ignore the first line of file, which is a header
    char *discard = nullptr;
    size_t discard_len = 0;
    getline(&discard, &discard_len, config_fd);
    free(discard);

    auto *p = static_cast<columns_s *>(malloc(sizeof(columns_s)));
    int n = 0;

    while (!feof(config_fd)) {
        if (int rv = fscanf(config_fd, "%d,%d,%d,%[^,],%[^,\n]", &(p + n)->ioa, &(p + n)->val, &(p + n)->dev, (p + n)->type, (p + n)->type_gi); rv != NUM_COLUMNS) {
            break;
        }
        p = static_cast<columns_s *>(realloc(p, sizeof(columns_s) * (++n + 1)));
    }

    spdlog::info("Read {:d} rows from {}", n, std::string(configFilename));

    auto *config = static_cast<config_t *>(malloc(sizeof(config_t))); // put config on heap
    //  config->len = n;

    auto _ioas = static_cast<int *>(malloc(sizeof(int) * n));
    auto _vals = static_cast<int *>(malloc(sizeof(int) * n));
    auto _devs = static_cast<int *>(malloc(sizeof(int) * n));
    auto _types = static_cast<int *>(malloc(sizeof(int) * n));
    auto _types_gi = static_cast<int *>(malloc(sizeof(int) * n));

    int j = 0;
    for (int i = 0; i < n; i++) {
        if (type_str_to_enum((p + i)->type) != -1 && type_str_to_enum((p + i)->type_gi) != -1) {
            _ioas[j] = (p + i)->ioa;
            _vals[j] = (p + i)->val;
            _devs[j] = (p + i)->dev;
            _types[j] = type_str_to_enum((p + i)->type);
            _types_gi[j] = type_str_to_enum((p + i)->type_gi);
            j++;
        } else {
            spdlog::warn("Skipping row {} with unrecognised type: {} or {}", i, (p + i)->type, (p + i)->type_gi);
        }
    }
    config->len = j;
    spdlog::info("Use {:d} points", j);
    config->ioas = _ioas;
    config->vals = _vals;
    config->devs = _devs;
    config->types = _types;
    config->types_gi = _types_gi;

    return config;
}

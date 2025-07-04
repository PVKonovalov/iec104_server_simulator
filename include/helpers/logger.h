//
// Created by Pavel Konovalov on 27/09/2024.
//

#ifndef LOGGER_H
#define LOGGER_H
#include "grpcpool/grpcpool.h"
#include "spdlog/spdlog.h"

inline void SetLogLevelAndPrefix(std::string &level, const std::string &prefix) {
    spdlog::level::level_enum log_level;

    if (level == "INFO") {
        log_level = spdlog::level::info;
    } else if (level == "WARN") {
        log_level = spdlog::level::warn;
    } else if (level == "ERROR") {
        log_level = spdlog::level::err;
    } else if (level == "DEBUG") {
        log_level = spdlog::level::debug;
    } else if (level == "TRACE") {
        log_level = spdlog::level::trace;
    } else if (level == "CRITICAL") {
        log_level = spdlog::level::critical;
    } else {
        log_level = spdlog::level::off;
    }

    spdlog::set_level(log_level);
}

inline std::string GetLogLevel() {
    switch (spdlog::get_level()) {
        case spdlog::level::info:
            return "INFO";
        case spdlog::level::warn:
            return "WARN";
        case spdlog::level::err:
            return "ERROR";
        case spdlog::level::debug:
            return "DEBUG";
        case spdlog::level::trace:
            return "TRACE";
        case spdlog::level::critical:
            return "CRITICAL";
        case spdlog::level::off:
            return "OFF";
        default: return "UNKNOWN";
    }
}
#endif //LOGGER_H

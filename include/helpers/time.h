//
// Created by Pavel Konovalov on 28/09/2024.
//

#ifndef TIME_H
#define TIME_H
#include <ctime>
#include <iec60870_common.h>
#include <iomanip>
#include <sstream>
#include <string>

inline std::tuple<tm, int> CP56Time2aToTmTime(CP56Time2a self) {
    tm tmTime{};

    tmTime.tm_sec = CP56Time2a_getSecond(self);
    tmTime.tm_min = CP56Time2a_getMinute(self);
    tmTime.tm_hour = CP56Time2a_getHour(self);
    tmTime.tm_mday = CP56Time2a_getDayOfMonth(self);
    tmTime.tm_mon = CP56Time2a_getMonth(self) - 1;
    tmTime.tm_year = CP56Time2a_getYear(self) + 100;

    return std::make_tuple(tmTime, CP56Time2a_getMillisecond(self));
}

/**
 * Generate a local ISO8601-formatted ('%Y-%m-%dT%H:%M:%S.%f%z') timestamp
 * and return as std::string localtime
 */
inline std::string GetCurrentISO8601TimeLocalString() {
    const auto now = std::chrono::system_clock::now();
    const auto itt = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::ostringstream ss;
    ss << std::put_time(localtime(&itt), "%FT%T") << '.' << std::setfill('0') << std::setw(3) << ms.count() << std::put_time(localtime(&itt), "%z");
    return ss.str();
}

inline uint64_t CurrentTimeSinceEpochMillisecond() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

/**
 * Create a local ISO8601-formatted ('%Y-%m-%dT%H:%M:%S.%f%z') from timestamp argument
 */
inline std::string ISO8601TimeLocalString(const int64_t timestamp) {
    const auto itt = static_cast<time_t>(timestamp) / 1000;
    const auto ms = timestamp % 1000;
    std::ostringstream ss;
    ss << std::put_time(localtime(&itt), "%FT%T") << '.' << std::setfill('0') << std::setw(3) << ms << std::put_time(localtime(&itt), "%z");
    return ss.str();
}

inline int64_t TmTimeSinceEpochMillisecond(const std::tm &time, const int milliseconds) {
    auto time_t_value = std::mktime(const_cast<std::tm *>(&time));
    if (time_t_value == -1) {
        return -1; // Error handling in case of invalid conversion
    }
    return static_cast<int64_t>(time_t_value) * 1000 + milliseconds;
}
#endif //TIME_H

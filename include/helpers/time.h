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
#include <chrono>
#include <stdexcept>


inline uint64_t CurrentTimeSinceEpochMillisecond() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

inline int64_t TmTimeSinceEpochMillisecond(const std::tm &time, const int milliseconds) {
    auto time_t_value = std::mktime(const_cast<std::tm *>(&time));
    if (time_t_value == -1) {
        return -1; // Error handling in case of invalid conversion
    }
    return static_cast<int64_t>(time_t_value) * 1000 + milliseconds;
}


inline bool SetHostClock(const std::tm& tmTime) {
    std::tm tmCopy = tmTime;
    std::time_t timeT = std::mktime(&tmCopy);

    if (timeT == -1) {
        return false;
    }

    const auto duration = std::chrono::seconds(timeT);
    auto timePoint = std::chrono::system_clock::time_point(duration);

#ifdef _WIN32
    SYSTEMTIME sysTime;
    FILETIME fileTime;
    ULARGE_INTEGER uli;

    auto timeSinceEpoch = timePoint.time_since_epoch();
    uli.QuadPart = (timeSinceEpoch.count() * 10000000LL) + 116444736000000000LL;

    fileTime.dwLowDateTime = uli.LowPart;
    fileTime.dwHighDateTime = uli.HighPart;

    if (!FileTimeToSystemTime(&fileTime, &sysTime)) {
        return false;
    }

    if (!SetSystemTime(&sysTime)) {
        return false;
    }
    return true;

#elif defined(__linux__) || defined(__unix__)
    struct timeval tv;
    tv.tv_sec = static_cast<time_t>(std::chrono::system_clock::to_time_t(timePoint));
    tv.tv_usec = 0;

    if (settimeofday(&tv, nullptr) != 0) {
        return false;
    }
    return true;

#elif defined(__APPLE__)
    struct timespec ts{};
    ts.tv_sec = static_cast<time_t>(std::chrono::system_clock::to_time_t(timePoint));
    ts.tv_nsec = 0;

    if (clock_settime(CLOCK_REALTIME, &ts) != 0) {
        return false;
    }
    return true;

#else
    return false; // Unsupported platform
#endif
}

inline tm CP56Time2aToTmTime(CP56Time2a self) {
    tm tmTime{};

    tmTime.tm_sec = CP56Time2a_getSecond(self);
    tmTime.tm_min = CP56Time2a_getMinute(self);
    tmTime.tm_hour = CP56Time2a_getHour(self);
    tmTime.tm_mday = CP56Time2a_getDayOfMonth(self);
    tmTime.tm_mon = CP56Time2a_getMonth(self) - 1;
    tmTime.tm_year = CP56Time2a_getYear(self) + 100;

    return tmTime;
}

inline std::string Iso8601TimeZ(const std::tm& tmTime) {
    std::stringstream ss;
    ss << std::put_time(&tmTime, "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

inline std::string Iso8601Time(const std::tm& tmTime) {
    std::stringstream ss;
    ss << std::put_time(&tmTime, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
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

inline uint64_t TimeSinceEpochMillisecond() {
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

#endif //TIME_H

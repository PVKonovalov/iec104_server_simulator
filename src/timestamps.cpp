#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

#include "timestamps.h"

#include "spdlog/spdlog.h"

static int *time_rate_ptr = nullptr;
static long *time_offset_ptr = nullptr;
static long *start_time_ptr = nullptr;


void init_time(const int *time_rate, const long *start_time) {
    time_rate_ptr = static_cast<int *>(malloc(sizeof(int)));
    time_offset_ptr = static_cast<long *>(malloc(sizeof(long)));
    start_time_ptr = static_cast<long *>(malloc(sizeof(long)));

    if (start_time == nullptr) {
        *time_offset_ptr = 0;
        *start_time_ptr = time(nullptr);
    } else {
        *time_offset_ptr = time(nullptr) - *start_time;
        *start_time_ptr = *start_time;
    }
    spdlog::info("Start time is: {:d}", *start_time_ptr);
    if (time_rate == nullptr) {
        *time_rate_ptr = 1;
    } else {
        *time_rate_ptr = *time_rate;
    }
    spdlog::info("Time multiplier: {:d}", *time_rate_ptr);
}

long get_time() {
    if (time_offset_ptr == nullptr ||
        time_rate_ptr == nullptr ||
        start_time_ptr == nullptr) {
        spdlog::info("Initialising time");
        init_time(nullptr, nullptr);
    }

    //get current time
    const long current = time(nullptr);

    //get elapsed time
    const long elapsed = (current - *time_offset_ptr) - *start_time_ptr;

    //adjust for time rate
    const long adjusted = *start_time_ptr + (elapsed * *time_rate_ptr);

    return adjusted;
}

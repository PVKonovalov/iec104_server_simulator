#include <string>
#include <iostream>
#include <sstream>

#include "utils.h"

#include <iomanip>

#include "cs104_slave.h"


extern int running;

void print_usage() {
    const std::string usage =
            "\nSimulator IEC104 server\n"
            "Simulates n IEC104 outstations\n\n"
            "usage: simulator_iec104_server [options]\n"
            "\toptions:\n"
            "\t-o, --outstations [int]\t\tSpecify number of outstations (default 5)\n"
            "\t-p, --port [int]\t\tPort number to start from (default 2404)\n"
            "\t-h, --host [str]\t\tHost address (default 0.0.0.0)\n"
            "\t-c, --common_address [int]\tCommon Address number to start from (default 1)\n"
            "\t-i, --interval [int]\t\tPeriodic transmission interval in seconds (default off)\n"
            "\t-f, --config_file [str]\t\tSpecify a csv file of measurement points\n"
            "\t-v, --verbose\t\t\tOutput additional logging information to STDOUT\n"
            "\t-s, --start_time [int]\t\tSpecify a start time in unix time (default system time)\n"
            "\t-m, --time_rate [int]\t\tSpecify a time multiplier (default 1)\n"
            "\t-S, --strict\t\t\tFail if any expected port is unavailable (default off)\n"
            "\t-T, --test_flags_off\t\tDisable test flags in outgoing IEC104 messages (default enabled)\n"
            "\t-H, --help\t\t\tDisplay instructions and exit\n";

    std::cout << usage << std::endl;
}


unsigned int init_rand() {
    auto seed = static_cast<unsigned int>(time(nullptr));
    srand(seed);
    return seed;
}


std::string CP56Time2aToString(const CP56Time2a time) {
    std::ostringstream oss;
    oss << std::setfill('0')
            << std::setw(2) << CP56Time2a_getHour(time) << ":"
            << std::setw(2) << CP56Time2a_getMinute(time) << ":"
            << std::setw(2) << CP56Time2a_getSecond(time) << " "
            << std::setw(2) << CP56Time2a_getDayOfMonth(time) << "/"
            << std::setw(2) << CP56Time2a_getMonth(time) << "/"
            << std::setw(4) << (CP56Time2a_getYear(time) + 2000);
    return oss.str();
}

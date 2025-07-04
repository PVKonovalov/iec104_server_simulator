#include <csignal>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>

#include "cs104_slave.h"
#include "hal_thread.h"
#include "hal_time.h"

#include "src/iec104_server_simulator.h"
#include "src/dummy_readings.h"
#include "src/utils.h"
#include "src/callbacks.h"
#include "src/config.h"
#include "src/timestamps.h"
#include "spdlog/spdlog.h"

int running = true;

extern "C" void signalHandler(int signum) {
    running = false;
    spdlog::error("Signal received: {}", signum);
    exit(0);
}

int main(const int argc, char **argv) {
    // catch Signals
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    std::signal(SIGINT, signalHandler);
    std::signal(SIGABRT, signalHandler);

    spdlog::set_pattern(std::string("[%Y/%m/%d %H:%M:%S.%e] [%l] %v"));

    // Define command line options
    int next_option;
    const struct option long_options[] = {
        {"help", no_argument, nullptr, 'H'},
        {"outstations", required_argument, nullptr, 'o'},
        {"host", required_argument, nullptr, 'h'},
        {"port", required_argument, nullptr, 'p'},
        {"common_address", required_argument, nullptr, 'c'},
        {"interval", required_argument, nullptr, 'i'},
        {"config_file", required_argument, nullptr, 'f'},
        {nullptr, 0, nullptr, 0}
    };

    // Command line option variables
    int outstations = 1;
    std::string host = "0.0.0.0";
    int start_port = 2404;
    int start_common_address = 1;
    int period_sec = 0;
    std::string config_file;
    int *time_rate = nullptr;

    // Get command line options
    do {
        const auto short_options = "H:o:h:p:c:i:f:";
        next_option = getopt_long(argc, argv, short_options, long_options, nullptr);
        switch (next_option) {
            case 'H':
                print_usage();
                exit(0);
            case 'o':
                outstations = atoi(optarg);
                break;
            case 'h':
                host = std::string(optarg);
                break;
            case 'p':
                start_port = atoi(optarg);
                break;
            case 'c':
                start_common_address = atoi(optarg);
                break;
            case 'i':
                period_sec = atoi(optarg);
                break;
            case 'f':
                config_file = std::string(optarg);
                break;
            case -1:
                break;
            default:
                print_usage();
                exit(0);
        }
    } while (next_option != -1);


    if (config_file.empty()) {
        spdlog::error("Missing configuration file name");
        print_usage();
        exit(1);
    }

    // Try to load configuration from csv file
    const config_t *config = get_config(config_file);

    // Initialise pseudo-time
    init_time(time_rate, 0);

    spdlog::info("Starting {:d} outstations, {:d} points, listening on {}:{:d}-{:d}", outstations, config->len, host.c_str(), start_port, start_port + outstations - 1);

    // Generate parameters for each server, and start it
    _params *params[outstations];
    pthread_t t_id[outstations];
    for (int i = 0; i < outstations; i++) {
        params[i] = static_cast<_params *>(malloc(sizeof(_params)));
        params[i]->port = i + start_port;
        params[i]->common_address = i + start_common_address;
        params[i]->host = reinterpret_cast<char *>(&host);
        params[i]->server_number = i;
        params[i]->period_sec = period_sec;
        params[i]->num_dummy_readings = config->len;
        params[i]->ioas = config->ioas;
        params[i]->vals = config->vals;
        params[i]->deviations = config->devs;
        params[i]->types = config->types;
        params[i]->types_gi = config->types_gi;

        pthread_create(&t_id[i], nullptr, server_spin_up, (void *) params[i]);
    }

    // Daemon threads, should only return on failure or SIGINT
    for (int i = 0; i < outstations; i++) {
        pthread_join(t_id[i], nullptr);
    }
}


static void *server_spin_up(void *args) {
    init_rand();

    sCS104_APCIParameters currentAPCIParameters = {
        /* .k = */ 12,
        /* .w = */ 8,
        /* .t0 = */ 30,
        /* .t1 = */ 15,
        /* .t2 = */ 10,
        /* .t3 = */ 20
    };

    auto *params = static_cast<_params *>(args);
    auto *connection_state = static_cast<_connection_state *>(malloc(sizeof(_connection_state)));
    connection_state->connected = false;
    connection_state->activated = false;
    connection_state->port = params->port;

    CS104_Slave slave = server_init(params, connection_state, args);

    CS104_Slave_start(slave);

    CS101_AppLayerParameters appLayerParameters = CS104_Slave_getAppLayerParameters(slave);

    spdlog::info("IEC104: OA:{:d} Size:[COT:{:d} CA:{:d} IOA:{:d}] Max ASDU:{:d}",
                 appLayerParameters->originatorAddress,
                 appLayerParameters->sizeOfCOT,
                 appLayerParameters->sizeOfCA,
                 appLayerParameters->sizeOfIOA,
                 // appLayerParameters->sizeOfTypeId,
                 // appLayerParameters->sizeOfVSQ,
                 appLayerParameters->maxSizeOfASDU);

    spdlog::info("IEC104: Windows:[W:{:d} K:{:d}] Time:[T0:{:d} T1:{:d} T2:{:d} T3:{:d}]",
                 currentAPCIParameters.w,
                 currentAPCIParameters.k,
                 currentAPCIParameters.t0,
                 currentAPCIParameters.t1,
                 currentAPCIParameters.t2,
                 currentAPCIParameters.t3);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    if (CS104_Slave_isRunning(slave) == false) {
        spdlog::error("Failed to start server on port {:d}", params->port);

        CS104_Slave_destroy(slave);
        Thread_sleep(500);
        running = false;
    } else {
        spdlog::info("Server started on port {:d}", params->port);
    }

    while (running) {
        if (params->period_sec && connection_state->activated) {
            Thread_sleep(1000 * params->period_sec);

            for (int i = 0; i < (params->num_dummy_readings); i++) {
                send_point(appLayerParameters, params, i, slave, CS101_COT_SPONTANEOUS);
            }
        } else {
            Thread_sleep(100);
        }
    }
    CS104_Slave_stop(slave);
    return nullptr;
}

static CS104_Slave server_init(_params *params, _connection_state *connection_state, void *args) {
    // create a new slave/server instance with default connection parameters and
    // default message queue size */
    CS104_Slave slave = CS104_Slave_create(10000, 10000);

    CS104_Slave_setLocalAddress(slave, params->host);
    CS104_Slave_setLocalPort(slave, params->port);


    /* Set mode to a single redundancy group
     * NOTE: library has to be compiled with CONFIG_CS104_SUPPORT_SERVER_MODE_SINGLE_REDUNDANCY_GROUP enabled (=1)
     */
    CS104_Slave_setServerMode(slave, CS104_MODE_SINGLE_REDUNDANCY_GROUP);


    /* set the callback handler for the interrogation command */
    CS104_Slave_setInterrogationHandler(slave, interrogationHandler, args);

    /* set handler to handle connection requests (optional) */
    CS104_Slave_setConnectionRequestHandler(slave, connectionRequestHandler, connection_state);

    /* set handler to track connection events (optional) */
    CS104_Slave_setConnectionEventHandler(slave, connectionEventHandler, connection_state);

    /* set the callback handler for the clock synchronization command */
    CS104_Slave_setClockSyncHandler(slave, clockSyncHandler, args);

    return slave;
}

static void send_point(CS101_AppLayerParameters alParams, _params *params, int i, CS104_Slave slave, CS101_CauseOfTransmission cot) {
    CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, cot, 0, params->common_address, false, false);

    InformationObject io = make_reading(
        params->vals[i],
        params->deviations[i],
        static_cast<TypeID>(params->types[i]),
        params->ioas[i]);

    CS101_ASDU_addInformationObject(newAsdu, io);
    InformationObject_destroy(io);
    CS104_Slave_enqueueASDU(slave, newAsdu);
    CS101_ASDU_destroy(newAsdu);

    spdlog::info("SEND SP ASDU:{:d} IOA:{:d}:{:d}", params->types[i], params->common_address, params->ioas[i]);
}

#pragma once
#include "cs104_slave.h"

#ifndef CALLBACKS_H
#define CALLBACKS_H

typedef struct _connection_state {
    int port;
    bool connected;
    bool activated;
} _connection_state;


typedef struct _params {
    char *host;
    int port;
    int common_address;
    int server_number;
    int period_sec;
    int num_dummy_readings;
    int *ioas;
    int *types;
    int *types_gi;
    float *vals;
    float *deviations;
} _params;


bool connectionRequestHandler(void *parameter, const char *ipAddress);

bool
interrogationHandler(void *parameter, IMasterConnection connection, CS101_ASDU asdu, uint8_t qoi);

void
connectionEventHandler(void *parameter, IMasterConnection con, CS104_PeerConnectionEvent event);

bool
clockSyncHandler(void *parameter, IMasterConnection connection, CS101_ASDU asdu, CP56Time2a newTime);

#endif

#pragma once
#include "cs104_slave.h"
#include "callbacks.h"

static CS104_Slave server_init(_params *params, _connection_state *connection_state, void * args);

static void *server_spin_up(void *args);

static void send_point(CS101_AppLayerParameters alParams, _params *params, int i, CS104_Slave slave, CS101_CauseOfTransmission cot);

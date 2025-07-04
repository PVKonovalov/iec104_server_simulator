#include <cstdio>
#include "callbacks.h"
#include "cs104_slave.h"
#include "dummy_readings.h"
#include "utils.h"
#include "spdlog/spdlog.h"

bool clockSyncHandler(void *parameter, IMasterConnection connection, CS101_ASDU asdu, CP56Time2a newTime) {
    auto *params = static_cast<_params *>(parameter);
    spdlog::warn("Port: {:d} Incoming time sync command with time {} - not implemented", params->port, CP56Time2aToString(newTime));
    return false;
}

static void getPeerIpAddress(IMasterConnection connection, char *ipAddress, int maxLen) {
    connection->getPeerAddress(connection, ipAddress, maxLen);
}

void connectionEventHandler(void *parameter, IMasterConnection con, CS104_PeerConnectionEvent event) {
    auto *connection_state = static_cast<_connection_state *>(parameter);
    char ipAddress[60];

    if (event == CS104_CON_EVENT_CONNECTION_OPENED) {
        connection_state->connected = true;
    } else if (event == CS104_CON_EVENT_CONNECTION_CLOSED) {
        connection_state->connected = false;
        connection_state->activated = false;
    } else if (event == CS104_CON_EVENT_ACTIVATED) {
        connection_state->activated = true;
    } else if (event == CS104_CON_EVENT_DEACTIVATED) {
        connection_state->activated = false;
    }

    getPeerIpAddress(con, ipAddress, sizeof(ipAddress));

    switch (event) {
        case CS104_CON_EVENT_CONNECTION_OPENED:
            spdlog::info("Connection opened {}:{:d}", ipAddress, connection_state->port);
            break;

        case CS104_CON_EVENT_CONNECTION_CLOSED:
            spdlog::info("Connection closed {}:{:d}", ipAddress, connection_state->port);
            break;

        case CS104_CON_EVENT_ACTIVATED:
            spdlog::info("Connection activated {}:{:d}", ipAddress, connection_state->port);
            break;

        case CS104_CON_EVENT_DEACTIVATED:
            spdlog::info("Connection deactivated {}:{:d}", ipAddress, connection_state->port);
            break;
    }
}

bool connectionRequestHandler(void *parameter, const char *ipAddress) {
    auto *connection_state = static_cast<_connection_state *>(parameter);
    if (connection_state->connected) {
        spdlog::warn("Rejecting connection request from {} because a client is already connected to port {:d}", ipAddress, connection_state->port);
        return false;
    }
    return true;
}

bool interrogationHandler(void *parameter, IMasterConnection connection, const CS101_ASDU asdu, uint8_t qoi) {
    const auto *params = static_cast<_params *>(parameter);
    const int common_address = params->common_address;
    const bool test_flags = false;
    const int num_dummy_readings = params->num_dummy_readings;
    int *ioas = params->ioas;
    int *types_gi = params->types_gi;
    const int *vals = params->vals;
    const int *deviations = params->deviations;

    spdlog::info("Port {:d} interrogation for group {:d}", params->port, qoi);

    if (qoi == 20) {
        /* only handle station interrogation */

        CS101_AppLayerParameters alParams = IMasterConnection_getApplicationLayerParameters(connection);

        IMasterConnection_sendACT_CON(connection, asdu, false);

        for (int i = 0; i < (num_dummy_readings); i++) {
            const CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION,
                                                         0, common_address, test_flags, false);

            InformationObject io = make_reading(vals[i], deviations[i], static_cast<TypeID>(types_gi[i]), ioas[i]);

            if (io != nullptr) {
                CS101_ASDU_addInformationObject(newAsdu, io);
                InformationObject_destroy(io);
                IMasterConnection_sendASDU(connection, newAsdu);
                CS101_ASDU_destroy(newAsdu);
                spdlog::info("SEND GI ASDU:{:d} IOA:{:d}:{:d}", params->types_gi[i], common_address, params->ioas[i]);
            } else {
                spdlog::error("Unsupported ASDU for index {:d}, type:{:d}", i, types_gi[i]);
            }
        }

        IMasterConnection_sendACT_TERM(connection, asdu);
    } else {
        IMasterConnection_sendACT_CON(connection, asdu, true);
    }

    return true;
}

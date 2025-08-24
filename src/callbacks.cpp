#include <cstdio>
#include "callbacks.h"
#include "cs104_slave.h"
#include "dummy_readings.h"
#include "utils.h"
#include "spdlog/spdlog.h"
#include "helpers/iec104.h"
#include "helpers/time.h"

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


bool asduHandler(void *parameter, IMasterConnection connection, CS101_ASDU asdu) {
    const TypeID asduType = CS101_ASDU_getTypeID(asdu);

    spdlog::info("command {}({:d}) received", TypeID_toString(asduType), static_cast<int>(asduType));

    InformationObject io = CS101_ASDU_getElement(asdu, 0);

    if (!io) {
        return false;
    }
    int address = InformationObject_getObjectAddress(io);
    auto cot = CS101_ASDU_getCOT(asdu);

    if (cot != CS101_COT_ACTIVATION) {
        spdlog::warn("unknown COT: {}({:d}) IOA: {:d} {}({:d})",
                     CS101_CauseOfTransmission_toString(cot),
                     static_cast<int>(cot),
                     address,
                     TypeID_toString(asduType),
                     static_cast<int>(asduType)
        );

        CS101_ASDU_setCOT(asdu, CS101_COT_UNKNOWN_COT);
        CS101_ASDU_setNegative(asdu, true);
        IMasterConnection_sendASDU(connection, asdu);
        InformationObject_destroy(io);

        return true;
    }

    switch (asduType) {
        case C_SC_NA_1: {
            const auto sc = reinterpret_cast<SingleCommand>(io);
            const int state = SingleCommand_getState(sc) ? 1 : 0;
            const bool isSelect = SingleCommand_isSelect(sc);

            spdlog::info("command {}({:d}) IOA: {:d} State: {}({:d}) Action: {}",
                         TypeID_toString(asduType),
                         static_cast<int>(asduType),
                         address,
                         (state ? "ON" : "OFF"),
                         state,
                         (isSelect ? "SELECT" : "EXECUTE"));

            CS101_ASDU_setCOT(asdu, CS101_COT_ACTIVATION_CON);
            IMasterConnection_sendASDU(connection, asdu);
            InformationObject_destroy(io);

            return true;
        }
        break;
        case C_SC_TA_1: {
            const auto sc = reinterpret_cast<SingleCommandWithCP56Time2a>(io);
            const int state = SingleCommand_getState(reinterpret_cast<SingleCommand>(sc)) ? 1 : 0;
            const bool isSelect = SingleCommand_isSelect(reinterpret_cast<SingleCommand>(sc));

            spdlog::info("command {}({:d}) IOA: {:d} State: {}({:d}) Action: {}",
                         TypeID_toString(asduType),
                         static_cast<int>(asduType),
                         address,
                         (state ? "ON" : "OFF"),
                         state,
                         (isSelect ? "SELECT" : "EXECUTE"));

            CS101_ASDU_setCOT(asdu, CS101_COT_ACTIVATION_CON);
            IMasterConnection_sendASDU(connection, asdu);
            InformationObject_destroy(io);

            return true;
        }
        break;

        case C_DC_NA_1: {
            const auto sc = reinterpret_cast<DoubleCommand>(io);
            const int state = DoubleCommand_getState(sc);
            const bool isSelect = DoubleCommand_isSelect(sc);

            spdlog::info("command {}({:d}) IOA: {:d} State: {}({:d}) Action: {}",
                         TypeID_toString(asduType),
                         static_cast<int>(asduType),
                         address,
                         DoubleCommandStateName(state),
                         state,
                         (isSelect ? "SELECT" : "EXECUTE"));


            CS101_ASDU_setCOT(asdu, CS101_COT_ACTIVATION_CON);
            IMasterConnection_sendASDU(connection, asdu);
            InformationObject_destroy(io);

            return true;
        }
        break;

        case C_DC_TA_1: {
            const auto sc = reinterpret_cast<DoubleCommandWithCP56Time2a>(io);
            const int state = DoubleCommand_getState(reinterpret_cast<DoubleCommand>(sc));
            const bool isSelect = DoubleCommand_isSelect(reinterpret_cast<DoubleCommand>(sc));

            spdlog::info("command {}({:d}) IOA: {:d} State: {}({:d}) Action: {}",
                         TypeID_toString(asduType),
                         static_cast<int>(asduType),
                         address,
                         DoubleCommandStateName(state),
                         state,
                         (isSelect ? "SELECT" : "EXECUTE"));


            CS101_ASDU_setCOT(asdu, CS101_COT_ACTIVATION_CON);
            IMasterConnection_sendASDU(connection, asdu);
            InformationObject_destroy(io);

            return true;
        }
        break;
        case C_CS_NA_1: // Set time
        {
            const auto newTimeCP56 = ClockSynchronizationCommand_getTime(reinterpret_cast<ClockSynchronizationCommand>(io));
            const auto newTime = CP56Time2aToTmTime(newTimeCP56);
            spdlog::info("C_CS_NA_1 clock synchronized command: {}", Iso8601Time(newTime));
            return true;
        }
        break;
        default: // Unknown ASDU type
            spdlog::error("unknown ASDU: {}({:d}) IOA: {:d}",
                          TypeID_toString(asduType),
                          static_cast<int>(asduType),
                          address);

            InformationObject_destroy(io);
            return false;
            break;
    }

    InformationObject_destroy(io);
    return false;
}

bool interrogationHandler(void *parameter, IMasterConnection connection, const CS101_ASDU asdu, uint8_t qoi) {
    const auto *params = static_cast<_params *>(parameter);
    const int common_address = params->common_address;
    const int num_dummy_readings = params->num_dummy_readings;
    int *ioas = params->ioas;
    int *types_gi = params->types_gi;
    const float *vals = params->vals;
    const float *deviations = params->deviations;

    spdlog::info("Port {:d} interrogation for group {:d}", params->port, qoi);

    if (qoi == 20) {
        /* only handle station interrogation */

        CS101_AppLayerParameters alParams = IMasterConnection_getApplicationLayerParameters(connection);

        IMasterConnection_sendACT_CON(connection, asdu, false);

        for (int i = 0; i < (num_dummy_readings); i++) {
            const CS101_ASDU newAsdu = CS101_ASDU_create(alParams, false, CS101_COT_INTERROGATED_BY_STATION,
                                                         0, common_address, false, false);

            InformationObject io = makeReading(vals[i], deviations[i], static_cast<TypeID>(types_gi[i]), ioas[i]);

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

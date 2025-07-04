//
// Created by Pavel Konovalov on 28/09/2024.
//

#ifndef GLOBALS_H
#define GLOBALS_H

#define APPLICATION_NAME std::string("salyut_iec104_client")
#define WEB_API_TIMEOUT_SEC (int) 30
#define WAITING_START_DT_CON_FROM_RTU_SEC (long long) 10
#define POINT_ID_IS_NOT_PRESENT (uint64_t)0xffffffffffffffff
#define OBJECT_IS_NOT_PRESENT (int)(-1)
#include "helpers/SharedQueue.h"

typedef struct RtdbDataPoint {
    uint64_t Address;
    double Value;
    uint64_t Quality;
    int64_t Timestamp;
    int64_t TimestampFep;
} RtdbDataPoint;

typedef enum {
    RTDB_POINT_TYPE_UNKNOWN = 0,
    RTDB_POINT_TYPE_BOOLEAN_OR_BIT_STRING = 1,
    RTDB_POINT_TYPE_FLOAT = 2,
    RTDB_POINT_TYPE_DOUBLE = 3,
    RTDB_POINT_TYPE_INTEGER = 4,
    RTDB_POINT_TYPE_SINGLE_COMMAND = 5,
    RTDB_POINT_TYPE_SINGLE_COMMAND_TWO_REGISTERS = 6,
    RTDB_POINT_TYPE_DOUBLE_COMMAND = 7
} RtdbPointType;

typedef struct AddressAndAsduKey {
    int Address;
    int Asdu;

    bool operator==(const AddressAndAsduKey &o) const {
        return Address == o.Address && Asdu == o.Asdu;
    }

    bool operator<(const AddressAndAsduKey &o) const {
        return Address < o.Address || (Address == o.Address && Asdu < o.Asdu);
    }
} AddressAndAsduKey;

inline SharedQueue<std::vector<RtdbDataPoint> > rtdbDataQueue;

#endif //GLOBALS_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "cs104_slave.h"

#include "dummy_readings.h"
#include "config.h"
#include "utils.h"
#include "timestamps.h"
#include "spdlog/spdlog.h"


bool sp_val() {
    return static_cast<bool>(rand() % 2);
}

DoublePointValue dp_val() {
    return static_cast<DoublePointValue>(rand() % 4);
}

int mvs_val(int val, int deviation) {
    int offset = rand() % (deviation + 1);
    if (rand() % 2) {
        val -= offset;
    } else {
        val += offset;
    }
    return val;
}

float mvn_val(float val, int deviation) {
    const float offset = rand() % (deviation + 1);

    if (rand() % 2) {
        val -= offset;
    } else {
        val += offset;
    }
    return val;
}

sCP56Time2a cp56Time2a() {
    const long time_epoch = get_time();

    const long time_ms = time_epoch * 1000;
    sCP56Time2a testTimestamp{};
    CP56Time2a_createFromMsTimestamp(&testTimestamp, time_ms);
    return testTimestamp;
}

BinaryCounterReading it_val(int32_t val, int deviation) {
    return BinaryCounterReading_create(nullptr, val, 1, false, false, false);
}

InformationObject make_reading(int val, int deviation, TypeID type, int ioa) {
    switch (type) {
        case M_SP_NA_1: /*  1 */ {
            return reinterpret_cast<InformationObject>(SinglePointInformation_create(nullptr, ioa, sp_val(), IEC60870_QUALITY_GOOD));
        }
        case M_DP_NA_1: /*  3 */ {
            return reinterpret_cast<InformationObject>(DoublePointInformation_create(nullptr, ioa, dp_val(), IEC60870_QUALITY_GOOD));
        }
        case M_ME_NA_1: /*  9 */ {
            return reinterpret_cast<InformationObject>(MeasuredValueNormalized_create(nullptr, ioa, mvn_val(val, deviation), IEC60870_QUALITY_GOOD));
        }
        case M_ME_NB_1: /* 11 */ {
            return reinterpret_cast<InformationObject>(MeasuredValueScaled_create(nullptr, ioa, mvs_val(val, deviation), IEC60870_QUALITY_GOOD));
        }
        case M_ME_NC_1: /* 13 */ {
            return reinterpret_cast<InformationObject>(MeasuredValueShort_create(nullptr, ioa, mvs_val(val, deviation), IEC60870_QUALITY_GOOD));
        }
        case M_IT_NA_1: /* 15 */ {
            return reinterpret_cast<InformationObject>(IntegratedTotals_create(nullptr, ioa, it_val(val, deviation)));
        }
        case M_ME_ND_1: /* 21 */ {
            return reinterpret_cast<InformationObject>(MeasuredValueNormalizedWithoutQuality_create(nullptr, ioa, mvn_val(val, deviation)));
        }
        case M_SP_TB_1: /* 30 */ {
            const long time_epoch = get_time();
            const long time_ms = time_epoch * 1000;
            sCP56Time2a testTimestamp{};
            CP56Time2a_createFromMsTimestamp(&testTimestamp, time_ms);
            return reinterpret_cast<InformationObject>(SinglePointWithCP56Time2a_create(nullptr, ioa, sp_val(), IEC60870_QUALITY_GOOD, &testTimestamp));
        }
        case M_DP_TB_1: /* 31 */ {
            const long time_epoch = get_time();
            const long time_ms = time_epoch * 1000;
            sCP56Time2a testTimestamp{};
            CP56Time2a_createFromMsTimestamp(&testTimestamp, time_ms);
            return reinterpret_cast<InformationObject>(DoublePointWithCP56Time2a_create(nullptr, ioa, dp_val(), IEC60870_QUALITY_GOOD, &testTimestamp));
        }
        case M_ME_TD_1: /* 34 */ {
            const long time_epoch = get_time();
            const long time_ms = time_epoch * 1000;
            sCP56Time2a testTimestamp{};
            CP56Time2a_createFromMsTimestamp(&testTimestamp, time_ms);
            return reinterpret_cast<InformationObject>(MeasuredValueNormalizedWithCP56Time2a_create(nullptr, ioa, mvn_val(val, deviation), IEC60870_QUALITY_GOOD, &testTimestamp));
        }
        case M_ME_TE_1: /* 35 */ {
            const long time_epoch = get_time();
            const long time_ms = time_epoch * 1000;
            sCP56Time2a testTimestamp{};
            CP56Time2a_createFromMsTimestamp(&testTimestamp, time_ms);
            return reinterpret_cast<InformationObject>(MeasuredValueScaledWithCP56Time2a_create(nullptr, ioa, mvs_val(val, deviation), IEC60870_QUALITY_GOOD, &testTimestamp));
        }
        case M_ME_TF_1: /* 36 */ {
            const long time_epoch = get_time();
            const long time_ms = time_epoch * 1000;
            sCP56Time2a testTimestamp{};
            CP56Time2a_createFromMsTimestamp(&testTimestamp, time_ms);
            return reinterpret_cast<InformationObject>(MeasuredValueShortWithCP56Time2a_create(nullptr, ioa, mvs_val(val, deviation), IEC60870_QUALITY_GOOD, &testTimestamp));
        }
        case M_IT_TB_1: /* 37 */ {
            const long time_epoch = get_time();
            const long time_ms = time_epoch * 1000;
            sCP56Time2a testTimestamp{};
            CP56Time2a_createFromMsTimestamp(&testTimestamp, time_ms);
            return reinterpret_cast<InformationObject>(IntegratedTotalsWithCP56Time2a_create(nullptr, ioa, it_val(val, deviation), &testTimestamp));
        }
        default: {
            spdlog::warn("Unsupported ASDU: {:d}", static_cast<int>(type));
        }
    }
    return nullptr;
}

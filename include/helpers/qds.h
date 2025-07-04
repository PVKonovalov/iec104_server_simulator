//
// Created by Pavel Konovalov on 30/09/2024.
//

#ifndef QDS_H
#define QDS_H
#include <cs101_information_objects.h>

inline QualityDescriptor FromRtdb(uint64_t quality) {
    return static_cast<QualityDescriptor>(quality & 0xff);
}

constexpr unsigned long long VSCommandDirect = 0x20000; // Direct command – used by the controlling station to immediately control operations in the controlled outstations
constexpr unsigned long long VSCommandSelect = 0x40000; // Check that the correct control operation is prepared
constexpr unsigned long long VSCommandExecute = 0x80000; // Execute the command
#endif //QDS_H

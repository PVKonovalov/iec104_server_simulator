//
// Created by Pavel Konovalov on 06/12/2024.
//

#ifndef IEC104_H
#define IEC104_H
#include <string>

inline std::string DoubleCommandStateName(const int state) {
    switch (state) {
        case 1:
            return "OFF";
        case 2:
            return "ON";
        default:
            return "NOT PERMITTED";
    }
}
#endif //IEC104_H

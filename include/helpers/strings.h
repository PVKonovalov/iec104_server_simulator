//
// Created by Pavel Konovalov on 27/09/2024.
//

#ifndef STRING_H
#define STRING_H
#include <string>
using namespace std;

// Convert each character to lowercase using tolower
inline string ToLower(const string &str) {
    string result;
    for (const char ch: str) {
        result += tolower(ch, std::locale());
    }

    return result;
}

// Convert each character to uppercase using toupper
inline string ToUpper(const string &str) {
    string result;
    for (const char ch: str) {
        result += toupper(ch, std::locale());
    }

    return result;
}

#endif //STRING_H

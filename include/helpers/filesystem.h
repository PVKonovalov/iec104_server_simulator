//
// Created by Pavel Konovalov on 27/09/2024.
//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <string>
#include <sys/stat.h>

namespace Filesystem {
    // Exists file
    inline bool Exists(const std::string &name) {
        struct stat buffer{};
        return (stat(name.c_str(), &buffer) == 0);
    }

    // Path to file without its filename
    inline std::string Path(const std::string &path) {
        const size_t pos = path.find_last_of("\\/");
        return (std::string::npos == pos) ? "" : path.substr(0, pos);
    }
}
#endif //FILESYSTEM_H

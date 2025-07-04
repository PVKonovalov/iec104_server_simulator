//
// Created by Pavel Konovalov on 01/10/2024.
//

#ifndef SHAREDMAP_H
#define SHAREDMAP_H
#include <map>
#include <mutex>
#include <condition_variable>

template<typename T>
class SharedMap {
public:
    void Put(uint64_t key, const T &item);

    bool Get(uint64_t key, T &item);

    void Begin();

    bool GetNext(T &item);

private:
    std::map<uint64_t, T> rtdb;
    std::mutex mutex;
    std::condition_variable cond;
    typename std::map<uint64_t, T>::iterator it;
};

template<typename T>
void SharedMap<T>::Put(const uint64_t key, const T &item) {
    std::lock_guard guard(mutex);
    rtdb[key] = item;
}

template<typename T>
bool SharedMap<T>::Get(const uint64_t key, T &item) {
    std::lock_guard guard(mutex);

    typename std::map<uint64_t, T>::iterator it = rtdb.find(key);

    if (it != rtdb.end()) {
        item = it->second;
        return true;
    }
    return false;
}

template<typename T>
void SharedMap<T>::Begin() {
    std::lock_guard guard(mutex);
    it = rtdb.begin();
}

template<typename T>
bool SharedMap<T>::GetNext(T &item) {
    std::lock_guard guard(mutex);

    if (it != rtdb.end()) {
        item = it->second;
        ++it;
        return true;
    }
    return false;
}
#endif //SHAREDMAP_H

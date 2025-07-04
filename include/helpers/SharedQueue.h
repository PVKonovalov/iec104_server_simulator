#ifndef SHAREDQUEUE_H
#define SHAREDQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class SharedQueue {
public:
    SharedQueue() : panic_(false) {
    }

    ~SharedQueue() = default;

    std::optional<T> Front(); // Use std::optional
    void Pop();

    void Push(const T &item);

    void Push(T &&item);

    int Size();

    void Cancel(); // Call it to stop waiting

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    bool panic_;
};

template<typename T>
void SharedQueue<T>::Cancel() {
    std::unique_lock lock(mutex_);
    panic_ = true;
    lock.unlock();
    cond_.notify_all();
}

template<typename T>
std::optional<T> SharedQueue<T>::Front() {
    std::unique_lock lock(mutex_);
    cond_.wait(lock, [this] { return !queue_.empty() || panic_; });

    if (panic_) {
        return std::nullopt; // Return null optional
    }

    return queue_.front();
}

template<typename T>
void SharedQueue<T>::Pop() {
    std::unique_lock lock(mutex_);
    cond_.wait(lock, [this] { return !queue_.empty() || panic_; });

    if (panic_) {
        return;
    }

    queue_.pop_front();
}

template<typename T>
void SharedQueue<T>::Push(const T &item) {
    std::unique_lock lock(mutex_);
    queue_.push_back(item);
    lock.unlock();
    cond_.notify_one();
}

template<typename T>
void SharedQueue<T>::Push(T &&item) {
    std::unique_lock lock(mutex_);
    queue_.push_back(std::move(item));
    lock.unlock();
    cond_.notify_one();
}

template<typename T>
int SharedQueue<T>::Size() {
    std::unique_lock lock(mutex_);
    return queue_.size();
}

#endif //SHAREDQUEUE_H

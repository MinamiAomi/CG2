#pragma once

#include <queue>
#include <mutex>

template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue(){}
    ThreadSafeQueue(const ThreadSafeQueue& copy) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_ = copy.queue_;
    }

    void Push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value));
    }

    bool TryPop(T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) { return false; }
        
        value = queue_.front();
        queue_.pop();
        return true;
    }


    bool Empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
private:
    std::queue<T> queue_;
    std::mutex mutex_;
};
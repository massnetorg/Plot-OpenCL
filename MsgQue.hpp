//
// Created by 余欣健 on 2019-01-29.
//

#ifndef POC_STRATUM_MSGQUEUE_HPP
#define POC_STRATUM_MSGQUEUE_HPP
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class MsgQueue {

public:
    MsgQueue() = default;
    MsgQueue(const MsgQueue&) = delete;
    MsgQueue &operator= (const MsgQueue&) = delete;

    void pop(T& elem);
    bool empty() const;

    void push(const T& elem);

    void push(T& elem) ;
private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

};

template <typename T>
void MsgQueue<T>::pop(T& elem) {
    std::unique_lock<std::mutex> lck(mutex_);
    cond_.wait(lck, [this]() {return !queue_.empty();});
    elem = std::move(queue_.front());
    queue_.pop();
}


template <typename T>
bool MsgQueue<T>::empty() const {
    std::lock_guard<std::mutex> lck(mutex_);
    return queue_.empty();
}

template <typename T>
void MsgQueue<T>::push(const T& elem) {
    {
        std::lock_guard<std::mutex> lck(mutex_);
        queue_.push(elem);
    }
    cond_.notify_one();
}


template <typename T>
void MsgQueue<T>::push(T& elem) {
    {
        std::lock_guard<std::mutex> lck(mutex_);
        queue_.push(std::move(elem));
    }
    cond_.notify_one();
}

#endif //POC_STRATUM_MSGQUEUE_HPP

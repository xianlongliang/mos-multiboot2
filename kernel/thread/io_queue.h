#pragma once

#include <std/stdint.h>
#include <std/list.h>
#include <std/lock_guard.h>
#include "mutex.h"
#include "task.h"
#include "semaphore.h"
#include "condition_variable.h"

template <class T>
class IOQueue
{
public:
    IOQueue(uint64_t max_queue_size) : max_queue_size(max_queue_size), queue() {}

    // ValType must match with T
    template<typename ValType>
    void Push(ValType &&val)
    {
        LockGuard lg(this->lock);
        if (this->queue.size() > this->max_queue_size) {
             return;
        }
        this->queue.push_front(std::forward<T>(val));
        this->cv.notify_one();
    }

    T Pop()
    {
        LockGuard lg(this->lock);
        this->cv.wait(this->lock, [this]() { return !this->Empty(); });
        auto back = this->queue.back();
        this->queue.pop_back();
        return back;
    }

    bool Empty() { return this->queue.empty(); }

private:
    Mutex lock;
    ConditionVariable cv;
    uint64_t max_queue_size;
    list<T> queue;
};
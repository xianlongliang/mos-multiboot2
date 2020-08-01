#pragma once

#include "noncopyable.h"

template <typename LockType>
class LockGuard : private noncopyable
{
public:
    LockGuard(LockType &lock) : lock(lock)
    {
        lock.lock();
    }
    ~LockGuard()
    {
        lock.unlock();
    }

private:
    LockType &lock;
};

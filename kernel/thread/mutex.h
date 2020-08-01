#pragma once

#include "semaphore.h"
#include "task.h"

class Mutex
{
public:
    Mutex();

    void lock();

    void unlock();

private:
    task_struct *owner;
    Semaphore sem;
};
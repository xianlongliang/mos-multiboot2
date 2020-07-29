#pragma once

#include "semaphore.h"
#include "task.h"

class Mutex
{
public:
    Mutex();

    void Lock();

    void Unlock();

private:
    task_struct *owner;
    Semaphore sem;
};
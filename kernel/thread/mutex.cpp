#include "mutex.h"
Mutex::Mutex() : owner(nullptr), sem(1) {}

void Mutex::lock()
{
    if (this->owner == current)
    {
        panic("recursive locking not allowed");
    }
    this->sem.Down();
    this->owner = current;
}

void Mutex::unlock()
{
    this->owner = nullptr;
    this->sem.Up();
}
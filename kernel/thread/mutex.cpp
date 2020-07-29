#include "mutex.h"
    Mutex::Mutex() : owner(nullptr), sem(1) {}

void Mutex::Lock() {
        if (this->owner == current) {
            panic("recursive locking not allowed");
        }
        this->sem.Down();
        this->owner = current;
    }

    void Mutex::Unlock() {
        this->owner = nullptr;
        this->sem.Up();
    }
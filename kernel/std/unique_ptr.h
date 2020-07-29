#pragma once

#include "noncopyable.h"
#include "move.h"

template <typename T>
class unique_ptr : noncopyable
{
public:
    unique_ptr(T *p) : ptr(p) {}
    ~unique_ptr() { delete ptr; }

    unique_ptr(unique_ptr<T> &&rv)
    {
        this->ptr = rv.ptr;
        rv.ptr = nullptr;
    }

    T *operator->() { return this->ptr; }
    T &operator*() { return *ptr; }
    
    T &operator[](int idx)
    {
        return ptr[idx];
    }

    T operator[](int idx) const
    {
        return ptr[idx];
    }

private:
    T *ptr;
};

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args &&... args)
{
    return unique_ptr<T>(new T(forward<Args>(args)...));
}

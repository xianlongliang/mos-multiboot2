#pragma once

#include "noncopyable.h"
#include "move.h"

namespace std
{
    template <typename T>
    class unique_ptr : noncopyable
    {
    public:
        unique_ptr(T *p = nullptr) : ptr(p) {}

        unique_ptr(unique_ptr<T> &&rhs)
        {
            this->ptr = rhs.ptr;
            rhs.ptr = nullptr;
        }

        ~unique_ptr() { delete ptr; }

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

        unique_ptr<T> operator=(unique_ptr<T> &&rhs)
        {
            this->reset();
            this->ptr = rhs.ptr;
            rhs.ptr = nullptr;
        }

        explicit operator bool()
        {
            return this->ptr != nullptr;
        }

        T *get()
        {
            return this->ptr;
        }

        T *release()
        {
            auto p = this->ptr;
            this->ptr = nullptr;
            return p;
        }

        void reset(T *p = nullptr)
        {
            delete this->ptr;
            this->ptr = p;
        }

    private:
        T *ptr;
    };

    template <typename T, typename... Args>
    unique_ptr<T> make_unique(Args &&... args)
    {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }

    template <class T>
    void swap(unique_ptr<T> &a, unique_ptr<T> &b) noexcept
    {
        swap(a.ptr, b.ptr);
    }
}; // namespace std
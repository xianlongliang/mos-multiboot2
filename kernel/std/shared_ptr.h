#pragma once

#include <std/atomic.h>
namespace std
{

    class shared_ptr_deleter_base
    {
    public:
        virtual void destroy() = 0;
    };

    template <class T>
    class shared_ptr_deleter : public shared_ptr_deleter_base
    {
    public:
        shared_ptr_deleter(T *p) : original_ptr(p) {}

        virtual void destroy() final
        {
            delete this->original_ptr;
        }

    private:
        T *original_ptr;
    };
    template <class T>
    class shared_ptr
    {

    public:
        shared_ptr() : deleter(nullptr), ptr(nullptr), strong_count(), weak_count() {}
        shared_ptr(T *p) : deleter(new shared_ptr_deleter<T>(p)), ptr(p), strong_count(1), weak_count(0) {}

        ~shared_ptr()
        {
            this->deleter->destroy();
        }

    private:
        shared_ptr_deleter_base *deleter;
        T *ptr;
        std::atomic<uint32_t> strong_count;
        std::atomic<uint32_t> weak_count;
    };
}; 
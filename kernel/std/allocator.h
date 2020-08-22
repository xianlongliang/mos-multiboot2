#pragma once

#include <std/stdint.h>
#include <std/new.h>
#include <memory/heap.h>

template <typename T>
struct default_allocator
{
    inline static void *allocate(size_t n)
    {
        return kmalloc(n * sizeof(T), 0);
    }

    inline static void deallocate(void *ptr)
    {
        kfree(ptr);
    }
};

template <typename T>
struct brk_allocator
{
    inline static void *allocate(size_t size)
    {
        return brk_up(size);
    }

    inline static void deallocate(void *ptr)
    {
        
    }
};
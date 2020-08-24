#pragma once

#include <std/stdint.h>
#include <std/new.h>
#include <memory/heap.h>
#include <memory/physical.h>
#include <memory/physical_page.h>

template <typename T>
struct default_allocator
{
    inline static void *allocate(size_t size)
    {
        return kmalloc(size * sizeof(T), 0);
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
        return brk_up(size * sizeof(T));
    }

    inline static void deallocate(void *ptr)
    {
        return;
    }
};

template <typename T>
struct buddy_system_allocator_oneshot
{
    inline static void *allocate(size_t size)
    {
        auto page_count = (size * sizeof(T)) / 4096;
        page_count = page_count == 0 ? 1 : page_count;
        auto page = PhysicalMemory::GetInstance()->Allocate(page_count, 0);
        return Phy_To_Virt(page->physical_address);
    }

    inline static void deallocate(void *ptr) {}
};
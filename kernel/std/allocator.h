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
    inline void *allocate(size_t size)
    {
        auto page_count = (double)(size * sizeof(T)) / (double)4096;
        auto page_alloc_count = (uint64_t)(__builtin_ceil(page_count));
        if (page_alloc_count == 0) page_alloc_count = 1;
        auto page = PhysicalMemory::GetInstance()->Allocate(page_alloc_count, 0);
        this->pages = page;
        return Phy_To_Virt(page->physical_address);
    }

    inline void deallocate(void *ptr)
    {
        PhysicalMemory::GetInstance()->Free(this->pages);
        this->pages = nullptr;
    }

private:
    Page *pages = nullptr;
};
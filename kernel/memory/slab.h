#pragma once
#include <std/list.h>
#include <std/stdint.h>
#include "physical_page.h"

struct SlabNode
{
    List list;
    Page *page;
    uint32_t used_count;
    uint32_t free_count;

    uint32_t bitmap_size;
    uint64_t *bitmap;
};

struct Slab
{
    uint32_t object_size;
    uint32_t total_used;
    uint32_t total_free;
    SlabNode *pool;

    void *Alloc();
    void Free(const void *ptr);
};

extern "C"
{
    Slab *slab_create(uint64_t object_size);
    int slab_free(Slab *slab);
}
#pragma once
#include <std/list.h>
#include <std/stdint.h>
#include <std/bitmap.h>

#include "physical_page.h"

struct Slab;

struct SlabNode
{
    List list;
    
    Slab *slab;
    uint8_t*vaddr;

    uint32_t used_count;
    uint32_t free_count;

    Bitmap* bitmap;
};

struct Slab
{
    uint32_t object_size;
    uint32_t total_used;
    uint32_t total_free;
    SlabNode *pool;

    uint8_t*Alloc();
    void Free(const uint8_t*ptr);
};

extern "C"
{
    Slab *slab_create(uint64_t object_size);
    int slab_free(Slab *slab);
}
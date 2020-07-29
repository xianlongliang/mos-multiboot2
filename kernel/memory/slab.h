#pragma once
#include <std/list.h>
#include <std/stdint.h>
#include <std/bitmap.h>

#include "physical_page.h"

class Slab;

struct SlabNode
{
    List list;
    
    Slab *slab;
    void *vaddr;

    uint32_t used_count;
    uint32_t free_count;

    Bitmap* bitmap2;
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
#pragma once

#include <std/stdint.h>
#include "defines.h"
#include "anchor.h"
#include <std/atomic.h>
#include "../heap.h"
#include <memory/physical.h>

// size of allocated block when allocating descriptors
// block is split into multiple descriptors
// 64k byte blocks
#define DESCRIPTOR_BLOCK_SZ (16 * PAGE)

class Descriptor;
class ProcHeap;

struct DescriptorNode
{
public:
    DescriptorNode(Descriptor *desc = nullptr)
    {
        this->split_val = {
            ._desc = desc,
            ._counter = 0
        };
    }

    union {
        struct
        {
            // ptr
            Descriptor *_desc = nullptr;
            // aba counter
            uint64_t _counter;
        } split_val;

        struct
        {
            uint128_t val;
        } full_val;
    };

public:
    void Set(Descriptor *desc, uint64_t counter)
    {
        // desc must be cacheline aligned
        // ASSERT(((uint64_t)desc & CACHELINE_MASK) == 0);
        // counter may be incremented but will always be stored in
        //  LG_CACHELINE bits
        this->split_val = {._desc = desc, ._counter = counter};
    }

    Descriptor *GetDesc() const
    {
        return (Descriptor *)(this->split_val._desc);
    }

    uint64_t GetCounter() const
    {
        return this->split_val._counter;
    }
};

// Superblock descriptor
// needs to be cache-line aligned
// descriptors are allocated and *never* freed
struct Descriptor
{
    // list node pointers
    // used in free descriptor list
    atomic<DescriptorNode> nextFree;
    // used in partial descriptor list
    atomic<DescriptorNode> nextPartial;
    // anchor
    atomic<Anchor> anchor;

    char *superblock;
    Page *superblock_page;
    ProcHeap *heap;
    uint32_t blockSize; // block size
    uint32_t maxcount;
};

extern atomic<DescriptorNode> AvailDesc;

Descriptor *DescAlloc();

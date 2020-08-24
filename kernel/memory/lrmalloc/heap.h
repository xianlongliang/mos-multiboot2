#pragma once

#include "size_class.h"
#include <std/atomic.h>
#include "descriptor.h"

struct SizeClassData;

struct ProcHeap
{
public:
    // ptr to descriptor, head of partial descriptor list
    atomic<DescriptorNode> partialList;
    // size class index
    size_t scIdx;

public:
    size_t GetScIdx() const { return scIdx; }
    SizeClassData *GetSizeClass() const
    {
        return get_size_class_via_index(scIdx);
    }
};

extern ProcHeap Heaps[MAX_SZ_IDX];

void lrmalloc_heap_init();

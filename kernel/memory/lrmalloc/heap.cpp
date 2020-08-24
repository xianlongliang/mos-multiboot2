#include "heap.h"

ProcHeap Heaps[MAX_SZ_IDX];

void lrmalloc_heap_init()
{
    for (size_t idx = 0; idx < MAX_SZ_IDX; ++idx)
    {
        ProcHeap &heap = Heaps[idx];
        heap.partialList.store(nullptr);
        heap.scIdx = idx;
    }
}
#include "lrmalloc.h"
#include "size_class.h"
#include "heap.h"
#include "../heap.h"
#include <smp/cpu.h>

// set true when smp init completed
static bool full_init;

extern atomic<DescriptorNode> AvailDesc;

void lrmalloc_init()
{
    size_class_init();
    AvailDesc.store({nullptr});
    lrmalloc_heap_init();
    this_cpu.mcache = TCache;
}

struct lrmalloc_meta
{
    Descriptor *desc;
};

void MallocFromNewSB(size_t scIdx, TCacheBin *cache, size_t &blockNum)
{
    ProcHeap *heap = &Heaps[scIdx];
    SizeClassData *sc = &SizeClasses[scIdx];

    Descriptor *desc = DescAlloc();

    uint32_t const blockSize = sc->blockSize;
    uint32_t const maxcount = sc->GetBlockNum();

    desc->heap = heap;
    desc->blockSize = blockSize;
    desc->maxcount = maxcount;
    desc->superblock = (char *)brk_up(sc->sbSize);

    // prepare block list
    char *superblock = desc->superblock;
    for (uint32_t idx = 0; idx < maxcount - 1; ++idx)
    {
        char *block = superblock + idx * blockSize;
        char *next = superblock + (idx + 1) * blockSize;
        *(char **)(block + sizeof(uint64_t)) = next;
        auto meta = (lrmalloc_meta*)block;
        meta->desc = desc;
    }

    // push blocks to cache
    char *block = superblock; // first block
    auto meta = (lrmalloc_meta*)block;
    cache->PushList(block, maxcount);

    Anchor anchor;
    anchor.avail = maxcount;
    anchor.count = 0;
    anchor.state = SB_FULL;

    desc->anchor.store(anchor);

    // register new descriptor
    // must be done before setting superblock as active
    // or leaving superblock as available in a partial list

    // if state changes to SB_PARTIAL, desc must be added to partial list
    blockNum += maxcount;
}

void FillCache(size_t scIdx, TCacheBin *cache)
{
    // at most cache will be filled with number of blocks equal to superblock
    size_t blockNum = 0;
    // use a *SINGLE* partial superblock to try to fill cache
    // MallocFromPartial(scIdx, cache, blockNum);
    // if we obtain no blocks from partial superblocks, create a new superblock
    if (blockNum == 0)
        MallocFromNewSB(scIdx, cache, blockNum);

    SizeClassData *sc = &SizeClasses[scIdx];
    (void)sc;
}

void *lrmalloc(size_t size)
{
    // size class calculation
    size_t scIdx = get_size_class(size + sizeof(lrmalloc_meta));

    TCacheBin *cache = &this_cpu.mcache[scIdx];
    // fill cache if needed
    if (cache->GetBlockNum() == 0)
        FillCache(scIdx, cache);

    return cache->PopBlock();
}

void *lrfree(const void *ptr)
{
    if (ptr == nullptr)
    {
        panic("freeing nullptr");
    }

    auto meta = (lrmalloc_meta *)(ptr - sizeof(uint64_t));
    auto cache = &this_cpu.mcache[meta->desc->heap->GetScIdx()];

    // flush cache if need
    // if (UNLIKELY(cache->GetBlockNum() >= sc->cacheBlockNum))
    //     FlushCache(scIdx, cache);

    cache->PushBlock((char *)ptr);
}
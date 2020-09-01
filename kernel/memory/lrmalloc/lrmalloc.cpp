#include "lrmalloc.h"
#include "size_class.h"
#include "heap.h"
#include "../heap.h"
#include <smp/cpu.h>

// set true when smp init completed
static bool full_init;

extern std::atomic<DescriptorNode> AvailDesc;

void lrmalloc_init()
{
    size_class_init();
    AvailDesc.store(nullptr);
    lrmalloc_heap_init();
    this_cpu->mcache = TCache;
}

struct lrmalloc_meta
{
    Descriptor *desc;
};

void DescRetire(Descriptor *desc)
{
    desc->blockSize = 0;
    DescriptorNode oldHead = AvailDesc.load();
    DescriptorNode newHead;
    do
    {
        desc->nextFree.store(oldHead);
        newHead.Set(desc, oldHead.GetCounter() + 1);
    } while (!AvailDesc.compare_exchange(oldHead, newHead));
}

Descriptor *HeapPopPartial(ProcHeap *heap)
{
    std::atomic<DescriptorNode> &list = heap->partialList;
    DescriptorNode oldHead = list.load();
    DescriptorNode newHead;
    do
    {
        Descriptor *oldDesc = oldHead.GetDesc();
        if (!oldDesc)
            return nullptr;

        newHead = oldDesc->nextPartial.load();
        newHead.Set(newHead.GetDesc(), oldHead.GetCounter() + 1);
    } while (!list.compare_exchange(oldHead, newHead));

    return oldHead.GetDesc();
}

void MallocFromPartial(size_t scIdx, TCacheBin *cache, size_t &blockNum)
{
    ProcHeap *heap = &Heaps[scIdx];

    Descriptor *desc = HeapPopPartial(heap);
    if (!desc)
        return;

    // reserve block(s)
    Anchor oldAnchor = desc->anchor.load();
    Anchor newAnchor;
    uint32_t maxcount = desc->maxcount;
    uint32_t blockSize = desc->blockSize;
    char *superblock = desc->superblock;

    // we have "ownership" of block, but anchor can still change
    // due to free()
    do
    {
        if (oldAnchor.state == SB_EMPTY)
        {
            DescRetire(desc);
            // retry
            return MallocFromPartial(scIdx, cache, blockNum);
        }

        // oldAnchor must be SB_PARTIAL
        // can't be SB_FULL because we *own* the block now
        // and it came from HeapPopPartial
        // can't be SB_EMPTY, we already checked
        // obviously can't be SB_ACTIVE
        // ASSERT(oldAnchor.state == SB_PARTIAL);

        newAnchor = oldAnchor;
        newAnchor.count = 0;
        // avail value doesn't actually matter
        newAnchor.avail = maxcount;
        newAnchor.state = SB_FULL;
    } while (!desc->anchor.compare_exchange(
        oldAnchor, newAnchor));

    // will take as many blocks as available from superblock
    // *AND* no thread can do malloc() using this superblock, we
    //  exclusively own it
    // if CAS fails, it just means another thread added more available blocks
    //  through FlushCache, which we can then use
    uint32_t blocksTaken = oldAnchor.count;
    uint32_t avail = oldAnchor.avail;

    // ASSERT(avail < maxcount);
    char *block = superblock + avail * blockSize;

    // cache must be empty at this point
    // and the blocks are already organized as a list
    // so all we need do is "push" that list, a constant time op
    // ASSERT(cache->GetBlockNum() == 0);
    cache->PushList(block, blocksTaken);

    blockNum += blocksTaken;
}

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
    // superblock is 4k aligned
    // thus two block in the same page owned by the same superblock
    auto page = PhysicalMemory::GetInstance()->Allocate(sc->sbSize / PAGE_4K_SIZE, 0);
    desc->superblock_page = page;
    desc->superblock = (char *)Phy_To_Virt(page->physical_address);

    // prepare block list
    char *superblock = desc->superblock;
    for (uint32_t idx = 0; idx < maxcount - 1; ++idx)
    {
        char *block = superblock + idx * blockSize;
        char *next = superblock + (idx + 1) * blockSize;
        *(char **)(block + sizeof(uint64_t)) = next;
        auto meta = (lrmalloc_meta *)block;
        meta->desc = desc;
    }

    // for the last block
    auto last_idx = maxcount - 1;
    char *last_block = superblock + last_idx * blockSize;
    *(char **)(last_block + sizeof(uint64_t)) = nullptr;
    ((lrmalloc_meta *)last_block)->desc = desc;

    // push blocks to cache
    char *block = superblock; // first block
    auto meta = (lrmalloc_meta *)block;
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

uint32_t ComputeIdx(char *superblock, char *block, size_t scIdx)
{
    SizeClassData *sc = &SizeClasses[scIdx];
    uint32_t scBlockSize = sc->blockSize;
    (void)scBlockSize; // suppress unused var warning

    // optimize integer division by allowing the compiler to create
    //  a jump table using size class index
    // compiler can then optimize integer div due to known divisor
    uint32_t diff = uint32_t(block - superblock);
    uint32_t idx = 0;
    switch (scIdx)
    {
#define SIZE_CLASS_bin_yes(index, blockSize) \
    case index:                              \
        idx = diff / blockSize;              \
        break;
#define SIZE_CLASS_bin_no(index, blockSize)
#define SC(index, lg_grp, lg_delta, ndelta, psz, bin, pgs, lg_delta_lookup) \
    SIZE_CLASS_bin_##bin((index + 1), ((1U << lg_grp) + (ndelta << lg_delta)))
        SIZE_CLASSES
    default:
        break;
    }
#undef SIZE_CLASS_bin_yes
#undef SIZE_CLASS_bin_no
#undef SC

    return idx;
}

void HeapPushPartial(Descriptor *desc)
{
    ProcHeap *heap = desc->heap;
    auto &list = heap->partialList;

    DescriptorNode oldHead = list.load();
    DescriptorNode newHead;
    do
    {
        newHead.Set(desc, oldHead.GetCounter() + 1);
        // ASSERT(oldHead.GetDesc() != newHead.GetDesc());
        newHead.GetDesc()->nextPartial.store(oldHead);
    } while (!list.compare_exchange(oldHead, newHead));
}

void FillCache(size_t scIdx, TCacheBin *cache)
{
    // at most cache will be filled with number of blocks equal to superblock
    size_t blockNum = 0;
    // use a *SINGLE* partial superblock to try to fill cache
    MallocFromPartial(scIdx, cache, blockNum);
    // if we obtain no blocks from partial superblocks, create a new superblock
    if (blockNum == 0)
        MallocFromNewSB(scIdx, cache, blockNum);

    SizeClassData *sc = &SizeClasses[scIdx];
    (void)sc;
}

// the blocks in the cache may comes from different superblock
void FlushCache(size_t scIdx, TCacheBin *cache)
{
    ProcHeap *heap = &Heaps[scIdx];
    SizeClassData *sc = &SizeClasses[scIdx];
    uint32_t const sbSize = sc->sbSize;
    uint32_t const blockSize = sc->blockSize;
    // after CAS, desc might become empty and
    //  concurrently reused, so store maxcount
    uint32_t const maxcount = sc->GetBlockNum();
    (void)maxcount; // suppress unused warning
    // @todo: optimize
    // in the normal case, we should be able to return several
    //  blocks with a single CAS
    while (cache->GetBlockNum() > 0)
    {
        char *head = cache->PeekBlock();
        char *tail = head;
        auto meta = (lrmalloc_meta *)head;
        Descriptor *desc = meta->desc;
        char *superblock = desc->superblock;

        // cache is a linked list of blocks
        // superblock free list is also a linked list of blocks
        // can optimize transfers of blocks between these 2 entities
        // by exploiting existing structure
        uint32_t blockCount = 1;
        // check if next cache blocks are in the same superblock
        // same superblock, same descriptor
        while (cache->GetBlockNum() > blockCount)
        {
            char *ptr = *(char **)(tail + sizeof(uint64_t));
            if (ptr < superblock || ptr >= superblock + sbSize)
                break; // ptr not in superblock

            // ptr in superblock, add to "list"
            ++blockCount;
            tail = ptr;
        }

        cache->PopList(*(char **)(tail + sizeof(uint64_t)), blockCount);

        // add list to desc, update anchor
        uint32_t idx = ComputeIdx(superblock, head, scIdx);

        Anchor oldAnchor = desc->anchor.load();
        Anchor newAnchor;
        do
        {
            // update anchor.avail
            char *next = (char *)(superblock + oldAnchor.avail * blockSize);
            *(char **)(tail + sizeof(uint64_t)) = next;

            newAnchor = oldAnchor;
            newAnchor.avail = idx;
            // state updates
            // don't set SB_PARTIAL if state == SB_ACTIVE
            if (oldAnchor.state == SB_FULL)
                newAnchor.state = SB_PARTIAL;
            // this can't happen with SB_ACTIVE
            // because of reserved blocks
            if (oldAnchor.count + blockCount == desc->maxcount)
            {
                newAnchor.count = desc->maxcount;
                newAnchor.state = SB_EMPTY; // can free superblock
            }
            else
                newAnchor.count += blockCount;
        } while (!desc->anchor.compare_exchange(oldAnchor, newAnchor));

        // after last CAS, can't reliably read any desc fields
        // as desc might have become empty and been concurrently reused
        // ASSERT(oldAnchor.avail < maxcount || oldAnchor.state == SB_FULL);
        // ASSERT(newAnchor.avail < maxcount);
        // ASSERT(newAnchor.count < maxcount);

        // CAS success, can free block
        if (newAnchor.state == SB_EMPTY)
        {
            // free superblock
            PhysicalMemory::GetInstance()->Free(desc->superblock_page);
        }
        else if (oldAnchor.state == SB_FULL)
            HeapPushPartial(desc);
    }
}
void *lrmalloc(size_t size)
{
    // size class calculation
    size_t scIdx = get_size_class(size + sizeof(lrmalloc_meta));
    TCacheBin *cache = &this_cpu->mcache[scIdx];
    // fill cache if needed
    if (cache->GetBlockNum() == 0)
        FillCache(scIdx, cache);

    return cache->PopBlock();
}

void lrfree(const void *ptr)
{
    if (ptr == nullptr)
    {
        panic("freeing nullptr");
    }

    auto meta = (lrmalloc_meta *)((int8_t*)ptr - sizeof(uint64_t));
    auto scIdx = meta->desc->heap->GetScIdx();
    auto sc = meta->desc->heap->GetSizeClass();
    auto cache = &this_cpu->mcache[scIdx];
    // flush cache if need
    if (cache->GetBlockNum() >= sc->cacheBlockNum)
        FlushCache(scIdx, cache);

    cache->PushBlock((char *)ptr);
}
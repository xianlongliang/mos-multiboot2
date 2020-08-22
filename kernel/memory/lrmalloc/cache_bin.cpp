#include "cache_bin.h"
#include "descriptor.h"

// cache_bin for bsp cpu
TCacheBin TCache[MAX_SZ_IDX];

void TCacheBin::PushBlock(char *block)
{
    // block has at least sizeof(char*)
    // sub 8 bytes point to it's origin offset
    block -= sizeof(uint64_t);
    *(char**)(block + sizeof(uint64_t)) = _block;
    _block = block;
    _blockNum++;
}

void TCacheBin::PushList(char *block, uint32_t length)
{
    // caller must ensure there's no available block
    // this op is only used to fill empty cache
    // ASSERT(_blockNum == 0);

    _block = block;
    _blockNum = length;
}

char *TCacheBin::PopBlock()
{
    // caller must ensure there's an available block
    // ASSERT(_blockNum > 0);

    // get the first block
    char *ret = _block;
    // advance to the next block
    _block = *(char **)(_block + sizeof(uint64_t));
    // decr counter
    _blockNum--;
    // return the first block
    return ret + sizeof(uint64_t);
}

void TCacheBin::PopList(char *block, uint32_t length)
{
    // ASSERT(_blockNum >= length);

    _block = block;
    _blockNum -= length;
}
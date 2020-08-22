#pragma once

#include <std/stdint.h>
#include "size_class.h"

extern SizeClassData SizeClasses[MAX_SZ_IDX];

// superblock states
// used in Anchor::state
enum SuperblockState
{
    // all blocks allocated or reserved
    SB_FULL = 0,
    // has unreserved available blocks
    SB_PARTIAL = 1,
    // all blocks are free
    SB_EMPTY = 2,
};

void *lrmalloc(size_t size);
void *lrfree(const void *ptr);

void lrmalloc_init();
//
// Created by mario on 2020/8/20.
//

/*
 * Copyright (C) 2019 Ricardo Leite. All rights reserved.
 * Licenced under the MIT licence. See COPYING file in the project root for details.
 */

#include "size_class.h"

#define PAGE 4096

#define SIZE_CLASS_bin_yes(blockSize, pages) \
    {blockSize, pages * PAGE},

#define SC(index, lg_grp, lg_delta, ndelta, psz, bin, pgs, lg_delta_lookup) \
    SIZE_CLASS_bin_yes((1U << lg_grp) + (ndelta << lg_delta), pgs)

#define LG_MAX_SIZE_IDX 6
// last size covered by a size class
// allocations with size > MAX_SZ are not covered by a size class
#define MAX_SZ ((1 << 13) + (1 << 11) * 3)

SizeClassData SizeClasses[MAX_SZ_IDX] = {
    {0, 0},
    SIZE_CLASSES};

void size_class_init()
{
    // each superblock has to contain several blocks
    // and it has to contain blocks *perfectly*
    //  e.g no space left after last block
    for (size_t scIdx = 1; scIdx < MAX_SZ_IDX; ++scIdx)
    {
        SizeClassData &sc = SizeClasses[scIdx];
        size_t blockSize = sc.blockSize;
        size_t sbSize = sc.sbSize;
        // size class large enough to store several elements
        if (sbSize > blockSize && (sbSize % blockSize) == 0)
            continue; // skip

        // increase superblock size so it can hold >1 elements
        while (blockSize >= sbSize)
            sbSize += sc.sbSize;

        sc.sbSize = sbSize;
    }

    // increase superblock size if need
    for (size_t scIdx = 1; scIdx < MAX_SZ_IDX; ++scIdx)
    {
        SizeClassData &sc = SizeClasses[scIdx];
        size_t sbSize = sc.sbSize;
        size_t mb2 = 1024 * 1024 * 2;
        // superblock alloc for at least 2MB
        while (sbSize < mb2)
            sbSize += sc.sbSize;

        sc.sbSize = sbSize;
    }

    // fill blockNum and cacheBlockNum
    for (size_t scIdx = 1; scIdx < MAX_SZ_IDX; ++scIdx)
    {
        SizeClassData &sc = SizeClasses[scIdx];
        // blockNum calc
        sc.blockNum = sc.sbSize / sc.blockSize;
        // cacheBlockNum calc
        sc.cacheBlockNum = sc.blockNum * 1;
    }
}

uint32_t get_size_class(uint32_t size)
{
    if (size > MAX_SZ)
        return 0;

    for (size_t scIdx = 1; scIdx < MAX_SZ_IDX - 1; ++scIdx)
    {
        auto &sc = SizeClasses[scIdx];
        auto &sc_next = SizeClasses[scIdx + 1];
        if (size >= sc.blockSize && size < sc_next.blockSize)
        {
            return scIdx + 1;
        }
    }

    return 0;
}

SizeClassData *get_size_class_via_index(uint32_t scIdx)
{
    return &SizeClasses[scIdx];
}
//
// Created by mario on 2020/8/20.
//

/*
 * Copyright (C) 2019 Ricardo Leite. All rights reserved.
 * Licenced under the MIT licence. See COPYING file in the project root for details.
 */

#include "size_class.h"

#define PAGE 4096
// size class data, from jemalloc 5.0
#define SIZE_CLASSES                                                      \
    /* index, lg_grp, lg_delta, ndelta, psz, bin, pgs, lg_delta_lookup */ \
    SC(0, 3, 3, 0, no, yes, 1, 3)                                         \
    SC(1, 3, 3, 1, no, yes, 1, 3)                                         \
    SC(2, 3, 3, 2, no, yes, 3, 3)                                         \
    SC(3, 3, 3, 3, no, yes, 1, 3)                                         \
                                                                          \
    SC(4, 5, 3, 1, no, yes, 5, 3)                                         \
    SC(5, 5, 3, 2, no, yes, 3, 3)                                         \
    SC(6, 5, 3, 3, no, yes, 7, 3)                                         \
    SC(7, 5, 3, 4, no, yes, 1, 3)                                         \
                                                                          \
    SC(8, 6, 4, 1, no, yes, 5, 4)                                         \
    SC(9, 6, 4, 2, no, yes, 3, 4)                                         \
    SC(10, 6, 4, 3, no, yes, 7, 4)                                        \
    SC(11, 6, 4, 4, no, yes, 1, 4)                                        \
                                                                          \
    SC(12, 7, 5, 1, no, yes, 5, 5)                                        \
    SC(13, 7, 5, 2, no, yes, 3, 5)                                        \
    SC(14, 7, 5, 3, no, yes, 7, 5)                                        \
    SC(15, 7, 5, 4, no, yes, 1, 5)                                        \
                                                                          \
    SC(16, 8, 6, 1, no, yes, 5, 6)                                        \
    SC(17, 8, 6, 2, no, yes, 3, 6)                                        \
    SC(18, 8, 6, 3, no, yes, 7, 6)                                        \
    SC(19, 8, 6, 4, no, yes, 1, 6)                                        \
                                                                          \
    SC(20, 9, 7, 1, no, yes, 5, 7)                                        \
    SC(21, 9, 7, 2, no, yes, 3, 7)                                        \
    SC(22, 9, 7, 3, no, yes, 7, 7)                                        \
    SC(23, 9, 7, 4, no, yes, 1, 7)                                        \
                                                                          \
    SC(24, 10, 8, 1, no, yes, 5, 8)                                       \
    SC(25, 10, 8, 2, no, yes, 3, 8)                                       \
    SC(26, 10, 8, 3, no, yes, 7, 8)                                       \
    SC(27, 10, 8, 4, no, yes, 1, 8)                                       \
                                                                          \
    SC(28, 11, 9, 1, no, yes, 5, 9)                                       \
    SC(29, 11, 9, 2, no, yes, 3, 9)                                       \
    SC(30, 11, 9, 3, no, yes, 7, 9)                                       \
    SC(31, 11, 9, 4, yes, yes, 1, 9)                                      \
                                                                          \
    SC(32, 12, 10, 1, no, yes, 5, no)                                     \
    SC(33, 12, 10, 2, no, yes, 3, no)                                     \
    SC(34, 12, 10, 3, no, yes, 7, no)                                     \
    SC(35, 12, 10, 4, yes, yes, 2, no)                                    \
                                                                          \
    SC(36, 13, 11, 1, no, yes, 5, no)                                     \
    SC(37, 13, 11, 2, yes, yes, 3, no)                                    \
    SC(38, 13, 11, 3, no, yes, 7, no)

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
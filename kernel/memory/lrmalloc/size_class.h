//
// Created by mario on 2020/8/20.
//

/*
 * Copyright (C) 2019 Ricardo Leite. All rights reserved.
 * Licenced under the MIT licence. See COPYING file in the project root for details.
 */

#pragma once

#include <std/stdint.h>

// number of size classes
// idx 0 reserved for large size classes
#define MAX_SZ_IDX 40

struct SizeClassData
{
public:
    // size of block
    uint32_t blockSize;
    // superblock size
    // always a multiple of page size
    uint32_t sbSize;
    // cached number of blocks, equal to sbSize / blockSize
    uint32_t blockNum;
    // number of blocks held by thread-specific caches
    uint32_t cacheBlockNum;

public:
    uint32_t GetBlockNum() const { return blockNum; }
};

// must be called before get_size_class
void size_class_init();
uint32_t get_size_class(uint32_t size);


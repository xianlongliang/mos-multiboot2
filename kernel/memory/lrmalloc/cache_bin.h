/*
 * Copyright (C) 2019 Ricardo Leite. All rights reserved.
 * Licenced under the MIT licence. See COPYING file in the project root for details.
 */

#pragma once

#include "size_class.h"

struct TCacheBin
{
private:
    char* _block = nullptr;
    uint32_t _blockNum = 0;

public:
    // common, fast ops
    void PushBlock(char* block);

    // push block list, cache *must* be empty
    void PushList(char* block, uint32_t length);

    char* PopBlock(); // can return nullptr

    // manually popped list of blocks and now need to update cache
    // `block` is the new head
    void PopList(char* block, uint32_t length);
    
    char* PeekBlock() const { return _block; }

    uint32_t GetBlockNum() const { return _blockNum; }

    // slow operations like fill/flush handled in cache user
};

extern TCacheBin TCache[MAX_SZ_IDX];
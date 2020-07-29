#pragma once

#include "stdint.h"

class Bitmap
{
public:
    Bitmap(uint32_t bitmap_size);
    Bitmap(uint32_t bitmap_size, bool placement_new);
    void Set(uint32_t offset);
    void UnSet(uint32_t offset);
    bool IsSet(uint32_t offset);
    void Clear();

    uint32_t Size() {
        return this->bitmap_size;
    }

    uint32_t BitSize() {
        return this->bit_size;
    }

private:
    // how many bits does bitmap have
    uint32_t bit_size;
    // how many bytes does bitmap have
    uint32_t bitmap_size;
    uint8_t bitmap[];
};
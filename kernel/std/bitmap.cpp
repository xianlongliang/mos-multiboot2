#include "bitmap.h"
#include "kstring.h"
#include "math.h"
#include <memory/kmalloc.h>

Bitmap::Bitmap(uint32_t bitmap_size)
{
    this->bitmap_size = bitmap_size;
    this->bit_size = bitmap_size * 8;
    this->Clear();
}

Bitmap::Bitmap(uint32_t bitmap_size, bool placement_new)
{
    this->bitmap_size = bitmap_size;
    this->bit_size = bitmap_size * 8;
    this->Clear();
}

void Bitmap::Set(uint32_t offset)
{
    auto idx = offset / 8;
    auto mod = offset % 8;
    SET_BIT(this->bitmap[idx], mod);
}

void Bitmap::UnSet(uint32_t offset)
{
    auto idx = offset / 8;
    auto mod = offset % 8;
    UNSET_BIT(this->bitmap[idx], mod);
}

bool Bitmap::IsSet(uint32_t offset)
{
    auto idx = offset / 8;
    auto mod = offset % 8;
    return CHECK_BIT(this->bitmap[idx], mod);
}

void Bitmap::Clear()
{
    bzero(this->bitmap, bitmap_size);
}
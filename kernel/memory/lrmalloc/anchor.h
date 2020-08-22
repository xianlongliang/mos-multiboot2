#pragma once

#include <std/stdint.h>

struct Anchor
{
    uint64_t state : 2;
    uint64_t avail : 31;
    uint64_t count : 31;
};
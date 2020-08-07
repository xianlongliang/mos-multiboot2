#pragma once

#include "stdint.h"

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define SET_BIT(var, pos) ((var) |= (1 << (pos)))
#define UNSET_BIT(var, pos) ((var) &= ~((uint64_t)1 << (pos)))

#define ROUND_UP_8BYTES(addr) (addr + (8 - 1)) & -8

inline uint64_t round_up_pow_of_2(uint64_t x) { return x == 1 ? 1 : 1 << (64 - __builtin_clzl(x - 1)); }

inline int64_t pow(int64_t base, int64_t exp)
{
    int64_t result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }
    return result;
}

template<class L, class R>
L& max(L a, R b) {
    return (a > b) ? a : b;
}
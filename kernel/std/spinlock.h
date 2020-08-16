#pragma once

#include <std/stdint.h>

class Spinlock
{
public:
    inline void lock()
    {
        asm volatile("movq $1, %%rcx	            \n\t"
                     "spin_lock_retry:              \n\t"
                     "xorq %%rax, %%rax	            \n\t"
                     "lock; cmpxchgq %%rcx, %0      \n\t"
                     "pause                         \n\t"
                     : "=m"(this->lock_val)
                     :: "memory", "rcx", "rax");
        asm volatile("jnz    spin_lock_retry    \n\t");
    }

    inline void unlock()
    {
        this->lock_val = 0;
    }

private:
    volatile uint64_t lock_val = 0; // 0:unlock, 1:lock
};

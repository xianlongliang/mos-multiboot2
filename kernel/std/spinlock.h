#pragma once

#include <std/stdint.h>

class Spinlock
{
public:
    inline void lock()  
    {
        asm volatile("movq $1, %rcx	            \n\t");
        asm volatile("spin_lock_retry:          \n\t");
        asm volatile("movq $0, %rax	            \n\t");
        asm volatile("lock; cmpxchgq %%rcx, %0  \n\t"
                     : "=m"(this->lock_val)
                     : "a"(0)
                     : "memory");
        asm volatile("pause");
        asm volatile("jnz    spin_lock_retry    \n\t");
    }

    inline void unlock()
    {
        this->lock_val = 0;
    }

private:
    volatile uint64_t lock_val = 0; // 0:unlock, 1:lock
};

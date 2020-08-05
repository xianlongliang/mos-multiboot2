#pragma once
#include <std/stdint.h>

class Spinlock
{
public:

    inline void lock()
    {
        asm volatile("1:	\n\t"
                     "lock	decq	%0	\n\t"
                     "jz	3f	\n\t"     // jump if lock == 0, exit the loop
                     "2:	\n\t"         // if lock != 0
                     "pause	\n\t"         // pause cpu
                     "cmpq	$0,	%0	\n\t" // check if lock still == 0
                     "jle	2b	\n\t"     // if true, pause cpu again
                     "jmp	1b	\n\t"     // or we try to acquire the lock all the way back again
                     "3:	\n\t"
                     : "=m"(this->lock_val)
                     :
                     : "memory");
    }
    
    inline void unlock()
    {
        asm volatile("movq	$1,	%0	\n\t"
                     : "=m"(this->lock_val)
                     :
                     : "memory");
    }

    inline uint64_t trylock()
    {
        uint64_t tmp_value = 0;
        asm volatile("xchgq	%0,	%1	\n\t"
                     : "=q"(tmp_value), "=m"(this->lock_val)
                     : "0"(0)
                     : "memory");
        return tmp_value;
    }

private:
    volatile uint64_t lock_val = 1; // 1:unlock, 0:lock
};

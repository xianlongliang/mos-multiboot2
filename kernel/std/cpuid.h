#pragma once

inline void get_cpuid(unsigned int Mop, unsigned int Sop, unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d)
{
    asm volatile("cpuid	\n\t"
                 : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                 : "0"(Mop), "2"(Sop));
}
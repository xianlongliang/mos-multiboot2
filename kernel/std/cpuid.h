#pragma once

inline void get_cpuid(unsigned int Mop, unsigned int Sop, unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d)
{
    asm volatile("cpuid	\n\t"
                 : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                 : "0"(Mop), "2"(Sop));
}

struct cpuid_struct
{
    uint32_t rax;
    uint32_t rbx;
    uint32_t rcx;
    uint32_t rdx;
};

inline cpuid_struct cpuid(uint32_t main_op)
{
    cpuid_struct ans;
    asm volatile(
        "mov %0, %%eax \n\t"
        "cpuid      \n\t"
        "mov %%eax, %0\n\t"
        "mov %%ebx, %1\n\t"
        "mov %%ecx, %2\n\t"
        "mov %%edx, %3\n\t"
        : "=r"(ans.rax), "=r"(ans.rbx), "=r"(ans.rcx), "=r"(ans.rdx)
        : "0"(main_op)
        : "rax", "rbx", "rcx", "rdx");
    return ans;
}
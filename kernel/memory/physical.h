#pragma once
#include <multiboot2.h>
#include <std/stdint.h>
#include <std/kstring.h>
#include "physical_page.h"
#include <std/printk.h>

#define flush_tlb()               \
    do                            \
    {                             \
        unsigned long tmpreg;     \
        __asm__ __volatile__(     \
            "movq	%%cr3,	%0	\n\t" \
            "movq	%0,	%%cr3	\n\t" \
            : "=r"(tmpreg)        \
            :                     \
            : "memory");          \
    } while (0)

inline void *Get_CR3()
{
    void *addr;
    __asm__ __volatile__(
        "movq	%%cr3,	%0	\n\t"
        : "=r"(addr)
        :
        : "memory");
    return addr;
}

inline void SET_CR3(void* pml4)
{
    // printk("setting: %p\n", pml4);
    asm volatile(
        "movq   %0,    %%rax   \n\t"
        "movq	%%rax, %%cr3   \n\t"
        :: "m"(pml4)
        : "memory");
}

class multiboot_mmap_entry;

constexpr uint64_t ZONES_RESERVED = 8;

class PhysicalMemory
{
public:

    static PhysicalMemory *GetInstance()
    {
        static PhysicalMemory instance;
        return &instance;
    }

    Page* Allocate(uint64_t count, uint64_t page_flags);
    void  Free(Page* page);
    
    inline static void* ZONE_VIRTUAL_START = 0x0;

private:
    friend void basic_init(void* mbi_addr);
    void Add(multiboot_mmap_entry *mmap);

    // only recognize 1 zones, 1-End
    Zone *zones;
};

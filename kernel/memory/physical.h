#pragma once
#include <multiboot2.h>
#include <std/stdint.h>
#include <std/string.h>

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

class multiboot_mmap_entry;
class Zone;

constexpr uint64_t ZONES_RESERVED = 8;

class PhysicalMemory
{
public:
    void Add(multiboot_mmap_entry *mmap);

    static PhysicalMemory *GetInstance()
    {
        static PhysicalMemory instance;
        return &instance;
    }

    inline static void* ZONE_VIRTUAL_START = 0x0;

private:
    // only recognize 2 zones, 0-1M and 1-End
    Zone *zones[ZONES_RESERVED];
    // how many zones are avaliable
    uint64_t zones_count = 0;
};

#pragma once
#include <multiboot2.h>
#include <std/stdint.h>
#include <std/string.h>
#include "zone.h"

// the virtual memory that kernel start, mapped to physical 0x0
constexpr uint64_t PAGE_OFFSET = 0xffffff8000000000;
constexpr uint64_t KETNEL_PAGE_OFFSET = PAGE_OFFSET;                               // alias
constexpr uint64_t KETNEL_MEM_BUDDY_SYSTEM_OFFSET = KETNEL_PAGE_OFFSET + 0x200000; // alias

constexpr uint16_t ENTRIES_PER_PAGE = 512;

enum BUDDY_ZONE
{
    BUDDY_ZONE_LOW_INDEX = 0,
    BUDDY_ZONE_NORMAL_INDEX = 1
};

#define PAGE_1G_SHIFT 30
#define PAGE_2M_SHIFT 21
#define PAGE_4K_SHIFT 12

#define PAGE_2M_SIZE (1UL << PAGE_2M_SHIFT)
#define PAGE_4K_SIZE (1UL << PAGE_4K_SHIFT)

#define PAGE_2M_MASK (~(PAGE_2M_SIZE - 1))
#define PAGE_4K_MASK (~(PAGE_4K_SIZE - 1))

#define PAGE_2M_ALIGN(addr) (((unsigned long)(addr) + PAGE_2M_SIZE - 1) & PAGE_2M_MASK)
#define PAGE_4K_ALIGN(addr) (((unsigned long)(addr) + PAGE_4K_SIZE - 1) & PAGE_4K_MASK)

#define Virt_To_Phy(addr) ((uint8_t *)(addr)-PAGE_OFFSET)
#define Phy_To_Virt(addr) ((uint8_t *)((uint8_t *)(addr) + PAGE_OFFSET))

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

class PhysicalMemory
{
public:
    void Add(multiboot_mmap_entry *e820);

    static PhysicalMemory *GetInstance()
    {
        static PhysicalMemory instance;
        return &instance;
    }

private:
    // only recognize 2 zones, 0-1M and 1-End
    Zone *zones[2];
    // how many zones are avaliable
    uint64_t zones_count = 0;
};

void memory_init();
Page *alloc_pages(BUDDY_ZONE buddy_index, uint32_t pages_count, uint64_t page_flags);
bool free_pages(Page *pages, uint32_t pages_count);
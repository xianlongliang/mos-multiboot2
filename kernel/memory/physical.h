#pragma once
#include <multiboot2.h>
#include <std/stdint.h>
#include <std/kstring.h>
#include "physical_page.h"
#include <std/printk.h>
#include <std/singleton.h>

#define flush_tlb()               \
    do                            \
    {                             \
        unsigned long tmpreg;     \
        asm volatile(             \
            "movq	%%cr3,	%0	\n\t" \
            "movq	%0,	%%cr3	\n\t" \
            : "=r"(tmpreg)        \
            :                     \
            : "memory");          \
    } while (0)

inline uint8_t *get_cr3()
{
    uint8_t *addr;
    asm volatile(
        "movq	%%cr3,	%0	\n\t"
        : "=r"(addr)
        :
        : "memory");
    return addr;
}

inline void set_cr3(void *pml4)
{
    // printk("setting: %p\n", pml4);
    asm volatile(
        "movq   %0,    %%rax   \n\t"
        "movq	%%rax, %%cr3   \n\t" ::"m"(pml4)
        : "memory");
}

class multiboot_mmap_entry;
class MBI2;

class PhysicalMemory : public Singleton<PhysicalMemory>
{
public:
    Page *Allocate(uint64_t count, uint64_t page_flags);
    void Free(Page *page);
    bool Reserve(uint64_t physical_address);

private:
    friend class MBI2;
    friend void basic_init(void *mbi_addr);
    void Add(multiboot_mmap_entry *mmap);

    List *zones_list;
};

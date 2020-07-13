#pragma once
#include <std/stdint.h>

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

class Zone;

struct Page
{
    Zone *zone;
    uint64_t physical_address;
    uint32_t reference_count;
    uint16_t attributes;
    uint16_t age;
};

class multiboot_mmap_entry;

class Zone
{
public:
    Zone(multiboot_mmap_entry *mmap);

    int64_t AllocatePages(uint64_t page_count);
    int64_t FreePages(uint64_t page_index);

    uint64_t BuddySystemSize()
    {
        // printk("total_pages_count %d\n", this->total_pages_count);
        return this->total_pages_count * sizeof(uint64_t) + sizeof(Zone);
    }

    uint64_t PageSize();

    inline Page *&Pages()
    {
        return this->pages;
    }

    inline uint64_t FreePagesCount()
    {
        return this->free_pages_count;
    }

    inline uint64_t UsedPagesCount()
    {
        return this->total_pages_count - this->free_pages_count;
    }

    inline uint64_t Span()
    {
        // memory layout:
        // Zone : nodes : pages
        return sizeof(Zone) + this->total_pages_count_rounded_up * 2 * sizeof(uint64_t) + this->total_pages_count_rounded_up * sizeof(Page);
    }

private:
    uint64_t free_pages_count;
    uint64_t total_pages_count;
    uint64_t total_pages_count_rounded_up;

    uint64_t physical_start_address;
    uint64_t physical_end_address;
    uint64_t attribute;

    uint64_t *nodes;
    Page *pages;
};

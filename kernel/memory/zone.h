#pragma once
#include <std/stdint.h>
#include "physical_page.h"

class multiboot_mmap_entry;

class Zone
{
public:
    Zone(multiboot_mmap_entry *mmap);

    int64_t AllocatePages(uint64_t page_count);
    int64_t FreePages(uint64_t page_index);
    bool Reserve(uint64_t page_index);
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

    uint32_t *nodes;
    Page *pages;
};

#pragma once
#include <std/stdint.h>

class Zone;

struct Page
{
    Zone *zone;
    uint64_t physical_address;
    uint32_t reference_count;
    uint16_t attributes;
    uint16_t age;
};

class Zone
{
public:
    
    void Construct(uint64_t page_count);
    int64_t AllocPages(uint64_t page_count);
    int64_t FreePages(uint64_t page_index);
    int64_t GetBeginIndex(uint64_t page_index);
    int64_t GetEndIndex(uint64_t page_index);

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

private:
    friend void memory_init();
    friend void set_page_attributes(Page *page, uint64_t flags);

    uint64_t free_pages_count;
    uint64_t total_pages_count;
    uint64_t total_avaliable_pages_count;

    uint64_t physical_start_address;
    uint64_t physical_end_address;
    uint64_t attribute;

    uint64_t total_reference_count;

    Page *pages;
    uint64_t nodes[0];
};


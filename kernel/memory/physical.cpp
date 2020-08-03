#include "physical.h"
#include <std/kstring.h>
#include <std/printk.h>
#include <std/debug.h>
#include <std/new.h>
#include "flags.h"
#include "zone.h"

void PhysicalMemory::Add(multiboot_mmap_entry *mmap)
{
    uint64_t start = PAGE_4K_ALIGN(mmap->addr);
    uint64_t end = ((mmap->addr + mmap->len) >> PAGE_4K_SHIFT) << PAGE_4K_SHIFT;
    printk("zone start: %p end: %p\n", start, end);
    if (start == 0x0)
        return;
    if (end <= start)
    {
        printk("mmap end <= start, drop\n");
        return;
    }
    this->zones = new ((void *)this->ZONE_VIRTUAL_START) Zone(mmap);
}

Page *PhysicalMemory::Allocate(uint64_t count, uint64_t page_flags)
{
    auto idx = this->zones->AllocatePages(count);
    if (idx != -1)
    {
        for (int i = idx; i < idx + count; ++i)
        {
            this->zones->Pages()[idx].attributes |= page_flags;
            this->zones->Pages()[idx].reference_count = 1;
        }
        printk("alloc: %p\n", this->zones->Pages()[idx].physical_address);
        return &this->zones->Pages()[idx];
    }
    return nullptr;
}

void PhysicalMemory::Free(Page *page)
{
    auto start_page = this->zones->Pages();
    auto diff = page - start_page;
    this->zones->FreePages(diff);
}

bool PhysicalMemory::Reserve(uint64_t physical_address)
{
    physical_address &= PAGE_4K_MASK;
    this->zones->Reserve((physical_address - 0x100000) / PAGE_4K_SIZE);
}

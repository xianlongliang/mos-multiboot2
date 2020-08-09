#include "physical.h"
#include <std/kstring.h>
#include <std/printk.h>
#include <std/debug.h>
#include <std/new.h>
#include "flags.h"
#include "zone.h"
#include <memory/heap.h>

void PhysicalMemory::Add(multiboot_mmap_entry *mmap)
{
    uint64_t start = PAGE_4K_ROUND_UP(mmap->addr);
    uint64_t end = (PAGE_4K_ROUND_DOWN(mmap->addr + mmap->len));
    printk("zone start: %p end: %p\n", start, end);
    printk("actual end: %p\n", mmap->addr + mmap->len);
    if (start == 0x0)
        return;
    if (end <= start)
    {
        printk("mmap end <= start, drop\n");
        return;
    }
    auto zone_addr = brk_up(sizeof(Zone));
    auto zone = new (zone_addr) Zone((void *)start, (void *)end);
    if (!zones_list)
    {
        this->zones_list = &zone->list_node;
    }
    else
    {
        list_add_to_behind(this->zones_list, &zone->list_node);
    }
}

Page *PhysicalMemory::Allocate(uint64_t count, uint64_t page_flags)
{
    auto zone = (Zone *)(this->zones_list);
    auto idx = zone->AllocatePages(count);
    if (idx != -1)
    {
        for (int i = idx; i < idx + count; ++i)
        {
            zone->Pages()[idx].attributes |= page_flags;
            zone->Pages()[idx].reference_count = 1;
        }
        printk("alloc: %p\n", zone->Pages()[idx].physical_address);
        return &zone->Pages()[idx];
    }
    return nullptr;
}

void PhysicalMemory::Free(Page *page)
{
    auto zone = (Zone *)(this->zones_list);
    auto start_page = zone->Pages();
    auto diff = page - start_page;
    zone->FreePages(diff);
}

bool PhysicalMemory::Reserve(uint64_t physical_address)
{
    auto zone = (Zone *)(this->zones_list);
    physical_address &= PAGE_4K_MASK_LOW;
    zone->Reserve((physical_address - 0x100000) / PAGE_4K_SIZE);
}

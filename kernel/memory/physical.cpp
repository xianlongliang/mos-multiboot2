#include "physical.h"
#include <std/kstring.h>
#include <std/printk.h>
#include <std/debug.h>
#include <std/new.h>
#include "flags.h"
#include "zone.h"
#include <memory/heap.h>

uint64_t PhysicalMemory::Add(multiboot_mmap_entry *mmap)
{
    uint64_t start = PAGE_4K_ROUND_UP(mmap->addr);
    uint64_t end = (PAGE_4K_ROUND_DOWN(mmap->addr + mmap->len));
    printk("zone start: %p end: %p\n", start, end);
    printk("actual end: %p\n", mmap->addr + mmap->len);

    if (end <= start)
    {
        printk("mmap end <= start, drop\n");
        return 0;
    }

    if (end - start < 0x8000000) {
        printk("region less than 128Mb, dropped\n");
        return 0;
    }

    if (start >= 0x100000000) {
        printk("todo: add map for 4G+ mem\n");
        return 0;
    }

    // auto zone_addr = brk_up(sizeof(Zone));
    auto valid_start = (uint64_t)Virt_To_Phy(brk_get());
    uint8_t* zone_addr = nullptr;
    if (start > valid_start) {
        zone_addr = (uint8_t*)start;
    }else if (start < valid_start && valid_start < end) {
        zone_addr = (uint8_t*)valid_start;
        start = valid_start;
    }else {
        return 0;
    }
    zone_addr = Phy_To_Virt(zone_addr);
    auto zone = new (zone_addr) Zone((uint8_t *)start, (uint8_t *)end);
    if (!zones_list)
    {
        this->zones_list = &zone->list_node;
    }
    else
    {
        list_add_to_behind(this->zones_list, &zone->list_node);
    }

    return zone->End();
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
        // printk("alloc: %p to %p\n", zone->Pages()[idx].physical_address, zone->Pages()[idx + count - 1].physical_address + 0x1000);
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
    return true;
}

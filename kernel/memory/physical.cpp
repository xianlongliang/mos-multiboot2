#include "physical.h"
#include <std/string.h>
#include <std/printk.h>
#include <std/debug.h>
#include <std/new.h>
#include "flags.h"
#include "zone.h"

void PhysicalMemory::Add(multiboot_mmap_entry *mmap)
{
    uint64_t start = PAGE_2M_ALIGN(mmap->addr);
    uint64_t end = ((mmap->addr + mmap->len) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;
    printk("zone start: %p end: %p\n", start, end);
    if (end <= start)
    {
        printk("mmap end <= start, drop\n");
        return;
    }
    this->zones[this->zones_count] = new ((void*)this->ZONE_VIRTUAL_START) Zone(mmap);
    this->zones_count += 1;
}

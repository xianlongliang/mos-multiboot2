#include "physical.h"
#include <std/string.h>
#include <std/printk.h>
#include <std/debug.h>
#include "flags.h"
#include "zone.h"

void PhysicalMemory::Add(multiboot_mmap_entry *e820)
{
    uint64_t start = PAGE_2M_ALIGN(e820->addr);
    uint64_t end = ((e820->addr + e820->len) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;
    printk("zone %d start: %p end: %p\n", this->zones_count, start, end);
    if (end <= start)
    {
        printk("mmap end <= start, drop\n");
        return;
    }
    uint64_t pages_count = (end - start) >> PAGE_2M_SHIFT;
}

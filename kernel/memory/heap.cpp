#include "heap.h"
#include "physical_page.h"

static void *brk = (void *)0x1000000 + PAGE_OFFSET;

void *brk_up(uint64_t size)
{
    printk("brk_up %d\n", size);
    auto res = brk;
    brk = brk + size;
    return res;
}


void *brk_down(uint64_t size)
{
    printk("brk_down %d\n", size);
    auto res = brk;
    brk = brk - size;
    return res;
}
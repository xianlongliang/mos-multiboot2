#include "heap.h"
#include "physical_page.h"
#include <std/math.h>

static uint8_t *brk;

void heap_init(void *addr)
{
    brk = (uint8_t *)addr;
}

void *brk_get()
{
    return brk;
}

void *brk_up(uint64_t size, uint64_t alignment)
{
    // printk("brk_up %d\n", size);
    uint8_t *res = nullptr;
    switch (alignment)
    {
    case 4096:
    {
        res = (uint8_t *)PAGE_4K_ROUND_UP((uint64_t)brk);
        break;
    }
    default:
    {
        res = (uint8_t *)ROUND_UP_16BYTES((uint64_t)brk);
        break;
    }
    }
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
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

void *brk_up(uint64_t size)
{
    // printk("brk_up %d\n", size);
    auto res = (uint8_t*)ROUND_UP_16BYTES((uint64_t)brk);
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
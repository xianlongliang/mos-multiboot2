#include "heap.h"
#include "physical_page.h"
#include <std/math.h>
#include <std/debug.h>

static uint8_t *brk;

// disable brk
static bool brk_done_flag;

void brk_init(void *addr)
{
    brk = (uint8_t *)addr;
}

void *brk_get()
{
    return brk;
}

void *brk_up(uint64_t size, uint64_t alignment)
{
    if (brk_done_flag)
        panic("use buddy system");
    // printk("brk_up %d\n", size);
    switch (alignment)
    {
    case 4096:
    {
        brk = (uint8_t *)PAGE_4K_ROUND_UP((uint64_t)brk);
        break;
    }
    default:
    {
        brk = (uint8_t *)ROUND_UP_16BYTES((uint64_t)brk);
        break;
    }
    }
    uint8_t *res = brk;
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

void brk_done()
{
    brk_done_flag = true;
}
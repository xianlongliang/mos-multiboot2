#include "tss.h"
#include <std/printk.h>
#include <arch/x86_64/kernel.h>

static tss_struct global_task_tss;

void set_tss(tss_struct &tss)
{
    global_task_tss = tss;
}

tss_struct &get_tss()
{
    return global_task_tss;
}

void tss_init()
{
    printk("tss_init\n");
    auto &tss = global_task_tss;
    tss.rsp0 = KERNEL_BOOT_STACK_ADDR;
    tss.rsp1 = 0;
    tss.rsp2 = 0;
    tss.ist1 = 0;
    tss.ist2 = 0;
    tss.ist3 = 0;
    tss.ist4 = 0;
    tss.ist5 = 0;
    tss.ist6 = 0;
    tss.ist7 = 0;
}

void set_gdt_tss(void *entry, void *tss_addr, uint16_t limit, uint16_t attr)
{
    auto addr = reinterpret_cast<uint64_t>(tss_addr);
    struct GDT_TSS *ts = (struct GDT_TSS *)(entry);

    ts->limit = limit; // 103
    ts->low[0] = (uint8_t)(addr);
    ts->low[1] = (uint8_t)(addr >> 8);
    ts->low[2] = (uint8_t)(addr >> 16);
    ts->attr = attr;
    ts->high[0] = (uint8_t)(addr >> 24);
    ts->high[1] = (uint8_t)(addr >> 32);
    ts->high[2] = (uint8_t)(addr >> 40);
    ts->high[3] = (uint8_t)(addr >> 48);
    ts->high[4] = (uint8_t)(addr >> 56);
}
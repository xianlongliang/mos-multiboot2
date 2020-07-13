#include "gdt.h"
#include <std/kstring.h>
#include <std/printk.h>
#include "tss.h"

static inline void load_gdt(struct GDTP *p)
{
    __asm__("lgdt %0" ::"m"(*p));
}

static inline void load_tr(uint16_t tr)
{
    __asm__ __volatile("ltr %0" ::"m"(tr));
}

void set_gdt_tss(int n, void *tss_addr, uint16_t limit, uint16_t attr)
{
    auto addr = reinterpret_cast<uint64_t>(tss_addr);
    struct GDT_TSS *ts = (struct GDT_TSS *)(&gdt_table[n]);

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

void gdt_init()
{
    tss_init();
    set_gdt_tss(7, &get_tss(), 103, 0x89);
    load_gdt(&gdt_ptr);
    load_tr(0x38);
    printk("gdt_init GDT_PTR %x\n", &gdt_ptr);
}
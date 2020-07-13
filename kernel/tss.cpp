#include "tss.h"
#include <std/printk.h>
#include <arch/x86_64/kernel.h>

static tss_struct global_task_tss;

void set_tss(tss_struct &tss)
{
    global_task_tss = tss;
}

tss_struct& get_tss() {
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
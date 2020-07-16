#include "syscall.h"
#include <std/msr.h>
#include <std/stdint.h>
#include <std/printk.h>
#include <thread/regs.h>
#include <tss.h>

static uint64_t syscall_entry(uint64_t syscall_number)
{
    auto i = get_tss();
    asm volatile("sysretq");
    return 0;
}

struct MSR_STAR_LAYOUT
{
    uint64_t noused : 32;
    uint64_t syscall_cs_ss : 16;
    uint64_t sysret_cs_ss : 16;
};

void syscall_init()
{
    /*  
        48-63bit: cs ss to be loaded when calling sysret
            actual value loaded: cs: field + 16, ss: field + 8
        32-47bit: cs ss to be loaded when calling sysret
            actual value loaded: cs: field, ss: field + 8
    */
    MSR_STAR_LAYOUT star = {0};
    star.sysret_cs_ss = 0x20;
    star.syscall_cs_ss = 0x08;
    wrmsr(MSR_STAR, *(uint64_t *)(&star));
    // set the syscall entry address
    wrmsr(MSR_LSTAR, (uint64_t(&syscall_entry)));
}
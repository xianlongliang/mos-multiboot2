#include "syscall.h"
#include <std/msr.h>
#include <std/stdint.h>
#include <std/printk.h>
#include <thread/regs.h>
#include <tss.h>
#include <memory/physical_page.h>

extern "C" uint64_t syscall_entry_c(uint64_t syscall_number)
{
    static uint64_t count = 0;
    printk("syscall %d times\n", count++);
    return 0;
}

struct MSR_STAR_LAYOUT
{
    uint64_t noused : 32;
    uint64_t syscall_cs_ss : 16;
    uint64_t sysret_cs_ss : 16;
};

extern "C" char pdpe_low;

struct CPU_STRUCT {
    void* syscall_stack;
    void* syscall_old_stack;
};

extern "C" void syscall_entry();

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

    auto cpu_struct = (CPU_STRUCT*)Phy_To_Virt(&pdpe_low);
    cpu_struct->syscall_stack = Phy_To_Virt((char*)&pdpe_low + 0x1000);
    wrmsr(MSR_KERNEL_GS_BASE, uint64_t(Phy_To_Virt((char*)&pdpe_low)));
}
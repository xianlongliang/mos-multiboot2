#include "syscall.h"
#include <std/msr.h>
#include <std/stdint.h>
#include <std/printk.h>
#include <thread/regs.h>
#include <tss.h>
#include <memory/physical_page.h>

extern "C" ssize_t sys_read(int fd, void *buf, size_t count);

extern "C" uint64_t syscall_entry_c(uint64_t syscall_number, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
    static uint64_t count = 0;
    printk("syscall %d times\n", count++);
    if (syscall_number == 1)
    {
        return sys_read(rsi, (void *)rdx, rcx);
    }
    return 0;
}

extern "C" void syscall_entry();

struct MSR_STAR_LAYOUT
{
    uint64_t noused : 32;
    uint64_t syscall_cs_ss : 16;
    uint64_t sysret_cs_ss : 16;
};

extern "C" char pdpe_low;

struct CPU_STRUCT
{
    void *syscall_stack;
    void *syscall_old_stack;
};

void Syscall::Init()
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

    // auto cpu_struct = (CPU_STRUCT *)Phy_To_Virt(&pdpe_low);
    // cpu_struct->syscall_stack = Phy_To_Virt((char *)&pdpe_low + 0x1000);
    // wrmsr(MSR_KERNEL_GS_BASE, uint64_t(Phy_To_Virt((char *)&pdpe_low)));

    // cli when syscall
    wrmsr(MSR_SYSCALL_MASK, (1 << 9));
}

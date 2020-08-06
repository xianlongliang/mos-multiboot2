#include "gdt.h"
#include <std/kstring.h>
#include <std/printk.h>
#include "tss.h"
#include <std/singleton.h>
#include <memory/kmalloc.h>
#include <smp/cpu.h>
#include <std/cpuid.h>

static inline void load_gdt(struct gdt_struct::GDTPointer *p)
{
    asm("lgdt %0" ::"m"(*p));
}

static inline void load_tr(uint16_t tr)
{
    asm volatile("ltr %0" ::"m"(tr));
}

struct cpuid_struct
{
    uint32_t rax;
    uint32_t rbx;
    uint32_t rcx;
    uint32_t rdx;
};

static cpuid_struct cpuid(uint32_t main_op)
{
    int a, b, c, d;
    asm volatile(
        "mov %0, %%eax \n\t"
        "cpuid      \n\t"
        "mov %%eax, %0\n\t"
        "mov %%ebx, %1\n\t"
        "mov %%ecx, %2\n\t"
        "mov %%edx, %3\n\t"
        : "=r"(a), "=r"(b), "=r"(c), "=r"(d)
        : "0"(main_op)
        : "rax", "rbx", "rcx", "rdx");
    return {a, b, c, d};
}

// void GDT::Init()
// {
//     tss_init();
//     set_gdt_tss(7, &get_tss(), 103, 0x89);
//     load_gdt(&gdt_ptr);
//     load_tr(0x38);
//     printk("gdt_init GDT_PTR %x\n", &gdt_ptr);
// }

void GDT::Init()
{
    auto cpuid_struct = cpuid(0x1);
    auto local_apic_id = cpuid_struct.rbx >> 24;
    auto cpu_struct = CPU::GetInstance()->Get(local_apic_id);
    load_gdt(&cpu_struct.gdt.gdt_ptr);
    load_tr(0x38);
    printk("gdt_init GDT_PTR %x\n", &gdt_ptr);
}
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
    auto& cpu_struct = CPU::GetInstance()->Get();
    load_gdt(&cpu_struct.gdt.gdt_ptr);
    load_tr(0x38);
    printk("CPU: %d GDT_PTR %p\n", cpu_struct.apic_id, &cpu_struct.gdt.gdt_ptr);
}
#include "smp.h"
#include <std/spinlock.h>
#include <interrupt/apic.h>
#include <interrupt/idt.h>
#include <gdt.h>

static Spinlock smp_lock;

void SMP::Init()
{
        GDT::GetInstance()->Init();

    smp_lock = Spinlock();

    auto apic = APIC::GetInstance();
    // DSH: 0x3 all excluding self
    // MT: 101b INIT
    // L: 1
    uint64_t init_ipi = 0xc4500;
    apic->ICR_Write((APIC::ICR_Register *)&init_ipi);

    extern char SMP_JMP;
    void *smp_entry_address = &SMP_JMP;
    printk("smp entry: %p\n", Virt_To_Phy(smp_entry_address));

    // DSH: 0x3 all excluding self
    // MT: 110b INIT
    // L: 1
    APIC::ICR_Register startup_ipi = {0};
    startup_ipi.DSH = 0x3;
    startup_ipi.MT = 0x6;
    startup_ipi.VEC = (uint64_t)Virt_To_Phy(smp_entry_address) >> PAGE_4K_SHIFT;
    apic->ICR_Write((APIC::ICR_Register *)&startup_ipi);
    printk("Startup IPI 1 Send\n");
    while (1)
    {
        asm volatile("cli");
        asm volatile("hlt");
    }
}

static void smp_apu_init()
{
}

extern "C" void smp_callback()
{
    smp_lock.lock();
    GDT::GetInstance()->Init();
    IDT::GetInstance()->Init();

    smp_apu_init();
    smp_lock.unlock();
    while (1)
        ;
}

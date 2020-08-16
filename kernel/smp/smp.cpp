#include "smp.h"
#include <std/spinlock.h>
#include <interrupt/apic.h>
#include <interrupt/idt.h>
#include <interrupt/pit.h>
#include <gdt.h>
#include <thread/task.h>
#include <syscall.h>

static Spinlock smp_lock;

void SMP::Init()
{
    GDT::GetInstance()->Init();
    IDT::GetInstance()->Init();
    APIC::GetInstance()->Init();
    Syscall::GetInstance()->Init();
    smp_lock = Spinlock();

    auto apic = APIC::GetInstance();
    // DSH: 0x3 all excluding self
    // MT: 101b INIT
    // L: 1
    uint64_t init_ipi = 0xc4500;
    apic->ICR_Write((APIC::ICR_Register *)&init_ipi);

    extern char SMP_JMP;
    auto smp_entry_address = (uint8_t *)&SMP_JMP;
    printk("smp entry: %p\n", smp_entry_address);

    auto cpus = CPU::GetInstance()->GetAll();
    for (int i = 1; i < cpus.size(); ++i)
    {
        auto &cpu = cpus[i];
        // DSH: 0x0 not broadcast
        // MT: 110b INIT
        // L: 1
        // VEC: cpu id to be waken up
        APIC::ICR_Register startup_ipi = {0};
        startup_ipi.DES = i;
        startup_ipi.DSH = 0x0;
        startup_ipi.MT = 0x6;
        startup_ipi.VEC = (uint64_t)smp_entry_address >> PAGE_4K_SHIFT;
        // try startup apu twice if the first try failed
        for (int j = 0; j < 2; ++j)
        {
            printk("Startup IPI %d Send\n", i);
            apic->ICR_Write((APIC::ICR_Register *)&startup_ipi);
            // give 100 ms for smp_callback to setup stack
            pit_spin(100);
            if (cpu.online)
                break;
        }

        if (!cpu.online)
        {
            printk("CPU %d startup failed\n", i);
        }
    }
}

extern "C" void smp_apu_init()
{
    smp_lock.lock();
    GDT::GetInstance()->Init();
    IDT::GetInstance()->Init();
    Syscall::GetInstance()->Init();
    APIC::GetInstance()->Init();

    CPU::GetInstance()->SetOnline();
    printk("AP CPU %d online\n", CPU::GetInstance()->Get().apic_id);
    smp_lock.unlock();
    asm volatile("sti");
    while (1)
    {
        asm volatile("hlt");
    }
}

extern "C" void smp_callback()
{
    auto stack = CPU::GetInstance()->Get().cpu_stack + PAGE_4K_SIZE - 0x10;
    // setup the stack
    asm volatile("movq %0, %%rsp \n\t"
                 "movq %%rsp, %%rbp \n\t" ::"m"(stack));
    //no return
    asm volatile("jmp smp_apu_init");
}

#include "apic.h"
#include <std/msr.h>
#include <std/port_ops.h>
#include <memory/heap.h>
#include <memory/physical_page.h>
#include <std/cpuid.h>
#include <memory/mapping.h>
#include "pit.h"
#include <std/interrupt.h>
#include <interrupt/idt.h>
#include <thread/scheduler.h>
#include <smp/cpu.h>

static void timer_callback(uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip)
{
    // printk("[%d] tick: %d\n", CPU::GetInstance()->Get().apic_id, tick++);
    static auto cpus = CPU::GetInstance();
    cpus->Get().scheduler.Schedule();
}

struct APIC_BASE_ADDR_REGISTER
{
    uint64_t MBZ : 8;
    uint64_t BSC : 1; // bootstrap cpu
    uint64_t MBZ2 : 2;
    uint64_t AE : 1;   // enable apic
    uint64_t ABA : 40; // apic base address
    uint64_t MBZ3 : 12;
};

struct APIC_ID_REGISTER
{
    uint32_t MBZ : 24;
    uint32_t AID : 8;
};

struct APIC_VERSION_REGISTER
{
    uint32_t VER : 8;
    uint32_t MBZ : 8;
    uint32_t MLE : 8;
    uint32_t MBZ2 : 7;
    uint32_t EAS : 1;
};

APIC::APIC()
{
}

void APIC::Init()
{
    if (!this->inited)
    {
        this->local_apic_base = (uint32_t *)brk_up(PAGE_4K_SIZE);
    }

    // mask all 8259a
    outb(0x21, 0xff);
    outb(0xA1, 0xff);

    // read IA32_APIC_BASE register
    auto apic_base_val = rdmsr(IA32_APIC_BASE);
    auto apic_base_addr_reg = (APIC_BASE_ADDR_REGISTER *)&apic_base_val;
    apic_base_addr_reg->BSC = 1;
    apic_base_addr_reg->AE = 1;
    apic_base_addr_reg->MBZ = 0;
    apic_base_addr_reg->MBZ2 = 0;
    apic_base_addr_reg->MBZ3 = 0;
    printk("APIC BASE: %x\n", apic_base_val);
    this->local_apic_base = (uint32_t *)Phy_To_Virt(apic_base_addr_reg->ABA << PAGE_4K_SHIFT);
    // vmap_frame_kernel(this->local_apic_base, (uint8_t*)(apic_base_addr_reg->ABA << PAGE_4K_SHIFT));

    wrmsr(IA32_APIC_BASE, apic_base_val);
    apic_base_val = rdmsr(IA32_APIC_BASE);
    // now apic is enabled
    auto apic_id_val = apic_read(APIC_ID);
    printk("APIC ID: %d\n", apic_id_val);
    auto apic_version_val = apic_read(APIC_VERSION);
    auto apic_version_reg = (APIC_VERSION_REGISTER *)&apic_version_val;
    printk("APIC Ver: %d, MLE: %d, EAS: %d\n", apic_version_reg->VER, apic_version_reg->MLE, apic_version_reg->EAS);
    // setup svr register
    // enable bit 8
    uint32_t apic_svr_val = (1 << 8);
    this->local_apic_base[APIC_SVR] = apic_svr_val;

    // clear error register
    this->local_apic_base[APIC_ESR] = 0;

    // mask lvt
    // just set bit 16 to 1
    uint32_t lvt_val = 0x10000;
    apic_write(APIC_LVT_TIMER, lvt_val);
    apic_write(APIC_LVT_THERMAL, lvt_val);
    apic_write(APIC_LVT_PERFORMANCE_MONITOR, lvt_val);
    apic_write(APIC_LVT_LINT0, lvt_val);
    apic_write(APIC_LVT_LINT1, lvt_val);
    apic_write(APIC_LVT_ERROR, lvt_val);

    apic_write(APIC_TPR, 1);

    timer_init();

    this->inited = true;
}

void APIC::EOI()
{
    apic_write(APIC_EOI, 0);
}

void APIC::timer_init()
{
    apic_write(APIC_TIMER_DCR, 0x0);
    apic_write(APIC_TIMER_ICR, 0xFFFFFFFF);
    static uint32_t ticks = 0xFFFFFFFF - apic_read(APIC_TIMER_CCR);
    if (!this->inited)
    {
        pit_spin(50);
    }

    // Start timer as periodic on IRQ 0, divider 16, with the number of ticks we counted
    apic_write(APIC_LVT_TIMER, IRQ0 | APIC_TIMER_PERIODIC);
    apic_write(APIC_TIMER_ICR, ticks);

    IDT::GetInstance()->Register(IRQ0, timer_callback);
}

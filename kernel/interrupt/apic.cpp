#include "apic.h"
#include <std/msr.h>
#include <std/port_ops.h>

APIC::APIC()
{
}

void APIC::Init()
{
    // read IA32_APIC_BASE register
    auto apic_base_val = rdmsr(IA32_APIC_BASE);
    // enable bit 10 and bit 11
    apic_base_val |= (1 << 10);
    apic_base_val |= (1 << 11);
    wrmsr(IA32_APIC_BASE, apic_base_val);
    // now apic is enabled

    // setup svr register
    auto apic_svr_val = rdmsr(IA32_APIC_SVR);
    // enable bit 8 and bit 12
    apic_svr_val |= (1 << 8);
    apic_svr_val |= (1 << 12);
    wrmsr(IA32_APIC_SVR, apic_svr_val);

    // mask lvt
    // just set bit 16 to 1
    auto lvt_val = 0x10000;
    wrmsr(IA32_APIC_LVT_CMCI, lvt_val);
    wrmsr(IA32_APIC_LVT_TIMER, lvt_val);
    wrmsr(IA32_APIC_LVT_THERMAL, lvt_val);
    wrmsr(IA32_APIC_LVT_LINT0, lvt_val);
    wrmsr(IA32_APIC_LVT_LINT1, lvt_val);
    wrmsr(IA32_APIC_LVT_ERROR, lvt_val);

    // mask all 8259a
    outb(0x21, 0xff);
    outb(0xA1, 0xff);
}
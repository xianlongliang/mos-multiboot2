#pragma once

#include <std/singleton.h>
#include <std/stdint.h>

class APIC : public Singleton<APIC>
{

public:
    enum Registers
    {
        ID = 0x20,
        VERSION = 0x30,
        TPR = 0x80,
        APR = 0x90,
        PPR = 0xa0,
        EOI_ = 0xb0,
        RRR = 0xb0,
        LDR = 0xd0,
        DFR = 0xe0,
        SVR = 0xf0,
        ISR0 = 0x100,
        ISR1 = 0x110,
        ISR2 = 0x120,
        ISR3 = 0x130,
        ISR4 = 0x140,
        ISR5 = 0x150,
        ISR6 = 0x160,
        ISR7 = 0x170,
        TMR0 = 0x180,
        TMR1 = 0x190,
        TMR2 = 0x1a0,
        TMR3 = 0x1b0,
        TMR4 = 0x1c0,
        TMR5 = 0x1d0,
        TMR6 = 0x1e0,
        TMR7 = 0x1f0,

        ESR = 0x280,
        ICR_LOW = 0x300 / 4,
        ICR_HIGH = 0x310 / 4,

    };
    APIC();

    // init local_apic and io_apic
    void Init();

    void EOI();

    struct ICR_Register
    {
        uint64_t VEC : 8;
        uint64_t MT : 3;
        uint64_t DM : 1;
        uint64_t DS : 1;
        uint64_t MBZ : 1;
        uint64_t L : 1;
        uint64_t TGM : 1;
        uint64_t RRS : 2;
        uint64_t DSH : 2;
        uint64_t MBZ1 : 36;
        uint64_t DES : 8;
    };

    void ICR_Write(ICR_Register *val)
    {
        auto high = *(uint64_t *)val >> 32;
        auto low = *(uint64_t *)val;
        this->apic_write(ICR_HIGH, high);
        this->apic_write(ICR_LOW, low);
    }

private:
    bool inited = false;
    uint32_t *local_apic_base;

    inline uint32_t apic_read(uint32_t reg)
    {
        uint32_t val = this->local_apic_base[reg];
        return val;
    }

    inline void apic_write(uint32_t reg, uint32_t val)
    {
        this->local_apic_base[reg] = val;
    }

    void timer_init();
};
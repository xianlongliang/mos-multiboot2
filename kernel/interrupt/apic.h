#pragma once

#include <std/singleton.h>
#include <std/stdint.h>

class APIC : public Singleton<APIC>
{

public:
    APIC();

    // init local_apic and io_apic
    void Init();
    
    void EOI();

private:
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
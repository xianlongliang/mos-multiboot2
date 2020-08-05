#pragma once

#include <std/stdint.h>
#include <std/singleton.h>

#define CODE_SEG 0x0008
#define DATA_SEG 0x0010

class GDT : public Singleton<GDT>
{
public:
    void Init();

    struct NO_ALIGNMENT GDTPointer
    {
        uint16_t limit;
        void *gdt_address;
    };

private:
    uint64_t gdt_table[10] = {
        0x0000000000000000,
        0x0020980000000000, // KERNEL CODE
        0x0000920000000000, // KERNEL DATA
        0x0000000000000000,
        0x0000000000000000,
        0x0000f20000000000, // USER DATA
        0x0020f80000000000, // USER CODE
        // ... tss (16 bytes)
    };

    GDTPointer gdt_ptr = {uint16_t(80 - 1), gdt_table};

    void set_gdt_tss(int n, void *tss_addr, uint16_t limit, uint16_t attr);

};

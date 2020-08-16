#pragma once

#include <std/stdint.h>
#include <std/singleton.h>

#define CODE_SEG 0x0008
#define DATA_SEG 0x0010

struct gdt_struct
{

    struct NO_ALIGNMENT GDTPointer
    {
        uint16_t limit;
        uint8_t *gdt_address;
    };

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

    GDTPointer gdt_ptr = {uint16_t(80 - 1), (uint8_t*)gdt_table};
};

class GDT : public Singleton<GDT>
{
public:
    void Init();
    void InitSMP();
    struct NO_ALIGNMENT GDTPointer
    {
        uint16_t limit;
        uint8_t *gdt_address;
    };
};

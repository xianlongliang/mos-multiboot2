#pragma once

#include <std/stdint.h>
#include <std/vector.h>
#include <std/singleton.h>

// place it in gdt
struct NO_ALIGNMENT GDT_TSS
{
    uint16_t limit;
    uint8_t low[3];
    uint16_t attr;
    uint8_t high[5];
    uint32_t reserved;
};

struct NO_ALIGNMENT tss_struct
{
    uint32_t reserved1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t io_map_base_addr;
};

class TSS : public Singleton<TSS>
{
public:
    void Add()
    {
        ap_tss.push_back(tss_struct());
    }

private:
    vector<tss_struct> ap_tss;
};

extern "C"
{
    void tss_init();

    void set_tss(tss_struct &tss);

    tss_struct &get_tss();

    void set_gdt_tss(void *entry, void *tss_addr, uint16_t limit, uint16_t attr);
}

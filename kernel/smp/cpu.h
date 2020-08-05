#pragma once

#include <std/stdint.h>
#include <std/vector.h>
#include <std/singleton.h>
#include <tss.h>
#include <gdt.h>
#include <memory/physical_page.h>

class CPU : public Singleton<CPU>
{
public:
    CPU() : cpus(1) {}

    uint64_t Count();

    void Add(uint64_t id)
    {
        this->cpus.push_back(cpu_struct());
        auto cs = &this->cpus.back();
        cs->apic_id = id;
        cs->cpu_stack = kmalloc(PAGE_4K_SIZE, 0);
        cs->tss = {0};
        cs->tss.rsp0 = (uint64_t)cs->cpu_stack;
        cs->gdt = gdt_struct();
        cs->gdt.gdt_ptr.gdt_address = &cs->gdt.gdt_table;
        cs->gdt.gdt_ptr.limit = uint16_t(80 - 1);
        set_gdt_tss(&cs->gdt.gdt_table[7], &cs->tss, 103, 0x89);
    }

    struct cpu_struct
    {
        uint64_t apic_id;
        void * cpu_stack;
        tss_struct tss;
        gdt_struct gdt;
    };

    cpu_struct &Get(uint64_t id)
    {
        for (int i = 0; i < this->cpus.size(); ++i)
        {
            if (this->cpus[i].apic_id == id)
                return this->cpus[i];
        }
    }

private:
    vector<cpu_struct> cpus;
};
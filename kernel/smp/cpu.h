#pragma once

#include <std/stdint.h>
#include <std/vector.h>
#include <std/singleton.h>
#include <tss.h>
#include <gdt.h>
#include <memory/physical_page.h>
#include <std/cpuid.h>
#include <std/kstring.h>
#include <thread/scheduler.h>
#include <memory/lrmalloc/cache_bin.h>

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
        cs->cpu_stack = (uint8_t *)brk_up(PAGE_4K_SIZE);
        bzero(cs->cpu_stack, PAGE_4K_SIZE);
        cs->tss = tss_struct();
        cs->tss.rsp0 = (uint64_t)cs->cpu_stack;
        cs->gdt = gdt_struct();
        cs->gdt.gdt_ptr.gdt_address = (uint8_t *)&cs->gdt.gdt_table;
        cs->gdt.gdt_ptr.limit = uint16_t(80 - 1);
        set_gdt_tss((uint8_t *)&cs->gdt.gdt_table[7], (uint8_t *)&cs->tss, 103, 0x89);
    }

    struct cpu_struct
    {
        bool online;
        uint64_t apic_id;
        uint8_t *cpu_stack;
        tss_struct tss;
        gdt_struct gdt;
        Scheduler scheduler;
        TCacheBin* mcache;
    };

    auto &GetAll()
    {
        return this->cpus;
    }

    cpu_struct &Get()
    {
        auto cpuid_struct = cpuid(0x1);
        auto local_apic_id = cpuid_struct.rbx >> 24;
        return this->cpus[local_apic_id];
    }

    cpu_struct &Get(uint64_t index)
    {
        return this->cpus[index];
    }

    void SetOnline()
    {
        auto cpuid_struct = cpuid(0x1);
        auto local_apic_id = cpuid_struct.rbx >> 24;
        this->cpus[local_apic_id].online = true;
    }

private:
    vector<cpu_struct, brk_allocator<cpu_struct>> cpus;
};

#define this_cpu CPU::GetInstance()->Get()
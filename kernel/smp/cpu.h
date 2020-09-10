#pragma once

#include <std/stdint.h>
#include <std/vector.h>
#include <std/singleton.h>
#include <tss.h>
#include <gdt.h>
#include <std/cpuid.h>
#include <std/kstring.h>
#include <thread/scheduler.h>
#include <memory/lrmalloc/cache_bin.h>
#include <std/msr.h>

struct cpu_struct
{
    cpu_struct* self;
    struct
    {
        void *syscall_stack;
        void *syscall_userland_stack;
    } syscall_struct = {0};

    bool online;
    uint64_t apic_id;
    void *cpu_stack;
    tss_struct tss;
    gdt_struct gdt;
    Scheduler scheduler;
    TCacheBin *mcache;
};

inline cpu_struct *get_this_cpu()
{
    cpu_struct *res;
    asm volatile("movq	%%gs:0,	%0	\n\t"
                 : "=r"(res)::"memory");
    return res;
}

#define this_cpu get_this_cpu()

class CPU : public Singleton<CPU>
{
public:
    CPU();

    uint64_t Count();

    void Add(uint64_t id)
    {
        printk("find cpu: %d\n", id);
        this->cpus.push_back(cpu_struct());
        auto cs = &this->cpus.back();
        cs->apic_id = id;
        cs->tss = tss_struct();
        cs->gdt = gdt_struct();
        cs->gdt.gdt_ptr.gdt_address = (uint8_t *)&cs->gdt.gdt_table;
        cs->gdt.gdt_ptr.limit = uint16_t(80 - 1);
        set_gdt_tss((uint8_t *)&cs->gdt.gdt_table[7], (uint8_t *)&cs->tss, 103, 0x89);

        cs->scheduler = Scheduler();
        cs->mcache = nullptr;
    }

    void Refresh() {
        for (int i = 0; i < this->cpus.size(); ++i) {
            auto& u = this->cpus[i];
            u.self = &this->cpus[i];
        }
    }

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
    vector<cpu_struct, buddy_system_allocator_oneshot<cpu_struct>> cpus;
};
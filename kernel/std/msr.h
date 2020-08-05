#pragma once
#include "stdint.h"

#define MSR_EFER 0xc0000080           /* extended feature register */
#define MSR_STAR 0xc0000081           /* legacy mode SYSCALL target */
#define MSR_LSTAR 0xc0000082          /* long mode SYSCALL target */
#define MSR_CSTAR 0xc0000083          /* compat mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084   /* EFLAGS mask for syscall */
#define MSR_FS_BASE 0xc0000100        /* 64bit FS base */
#define MSR_GS_BASE 0xc0000101        /* 64bit GS base */
#define MSR_KERNEL_GS_BASE 0xc0000102 /* SwapGS GS shadow */
#define MSR_TSC_AUX 0xc0000103        /* Auxiliary TSC */

#define IA32_APIC_BASE 0x0000001b

#define APIC_ID 0x20 / 4
#define APIC_VERSION 0x30 / 4
#define APIC_TPR 0x80 / 4
#define APIC_EOI 0xb0 / 4
#define APIC_SVR 0xF0 / 4
#define APIC_ESR 0x280 / 4

#define APIC_LVT_TIMER 0x320 / 4
#define APIC_LVT_THERMAL 0x330 / 4
#define APIC_LVT_PERFORMANCE_MONITOR 0x340 / 4
#define APIC_LVT_LINT0 0x350 / 4
#define APIC_LVT_LINT1 0x360 / 4
#define APIC_LVT_ERROR 0x370 / 4
#define APIC_TIMER_ICR 0x380 / 4
#define APIC_TIMER_CCR 0x390 / 4
#define APIC_TIMER_DCR 0x3e0 / 4
#define APIC_TIMER_PERIODIC 0x00020000
#define APIC_TIMER_ONE_SHOT 0x00000000

inline void wrmsr(unsigned long address, unsigned long value)
{
    asm volatile("wrmsr	\n\t" ::"d"(value >> 32), "a"(value & 0xffffffff), "c"(address)
                     : "memory");
}

inline uint64_t rdmsr(uint64_t address)
{
    uint64_t tmp0 = 0;
    uint64_t tmp1 = 0;
    asm volatile("rdmsr	\n\t"
                 : "=d"(tmp0), "=a"(tmp1)
                 : "c"(address)
                 : "memory");
    return tmp0 << 32 | tmp1;
}
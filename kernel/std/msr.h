#pragma once


#define MSR_EFER 0xc0000080           /* extended feature register */
#define MSR_STAR 0xc0000081           /* legacy mode SYSCALL target */
#define MSR_LSTAR 0xc0000082          /* long mode SYSCALL target */
#define MSR_CSTAR 0xc0000083          /* compat mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084   /* EFLAGS mask for syscall */
#define MSR_FS_BASE 0xc0000100        /* 64bit FS base */
#define MSR_GS_BASE 0xc0000101        /* 64bit GS base */
#define MSR_KERNEL_GS_BASE 0xc0000102 /* SwapGS GS shadow */
#define MSR_TSC_AUX 0xc0000103        /* Auxiliary TSC */

#define IA32_APIC_BASE 0x1b
#define IA32_APIC_SVR 0x80f
#define IA32_APIC_LOCAL_ID 0x802

#define IA32_APIC_LVT_CMCI 0x82f
#define IA32_APIC_LVT_TIMER 0x832
#define IA32_APIC_LVT_THERMAL 0x833
#define IA32_APIC_LVT_PERFORMANCE_MONITOR 0x834
#define IA32_APIC_LVT_LINT0 0x835
#define IA32_APIC_LVT_LINT1 0x836
#define IA32_APIC_LVT_ERROR 0x837

inline void wrmsr(unsigned long address, unsigned long value)
{
    asm __volatile__("wrmsr	\n\t" ::"d"(value >> 32), "a"(value & 0xffffffff), "c"(address)
                     : "memory");
}

inline unsigned long rdmsr(unsigned long address)
{
    unsigned int tmp0 = 0;
    unsigned int tmp1 = 0;
    asm __volatile__("rdmsr	\n\t"
                     : "=d"(tmp0), "=a"(tmp1)
                     : "c"(address)
                     : "memory");
    return (unsigned long)tmp0 << 32 | tmp1;
}
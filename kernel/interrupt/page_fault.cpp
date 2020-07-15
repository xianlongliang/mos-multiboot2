#include <std/stdint.h>
#include <std/printk.h>

inline static uint64_t GetCR2()
{
    uint64_t cr2_val;
    asm volatile(
        "movq %%cr2, %%rdi \n\t"
        "movq %%rdi, %0"
        : "=m"(cr2_val));
    return cr2_val;
}

// from AMD64 Architecture Programmer’s Manual, Volume 2: System Programming (Page-Fault Error Code)
struct page_fault_error_code
{
    uint32_t P : 1;
    uint32_t R_W : 1;
    uint32_t U_S : 1;
    uint32_t RSV : 1;
    uint32_t I_D : 1;
    uint32_t PK : 1;
    uint32_t Reserved : 25;
    uint32_t RMP : 1;
};

extern "C" void page_fault_handler(uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip)
{
    uint64_t test = 0;
    auto pfec = (page_fault_error_code *)&test;
    printk("page_fault RIP: %p\n", rip);
    printk("page_fault address CR2: %p\n", GetCR2());
    printk("page_fault error_code %x\n", error_code);
    printk("P:%d R_W:%d U_S:%d RSV:%d I_D:%d PK:%d\n",
           pfec->P, pfec->R_W, pfec->U_S, pfec->RSV, pfec->I_D, pfec->PK);
    printk("RMP:%d\n", pfec->RMP);

}
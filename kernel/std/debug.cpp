#include "debug.h"
#include "elf/elf-em.h"
#include "printk.h"

// the rbp when we jump to kernel_start
constexpr uint64_t start_kernel_base = 0x7000;

static ELFDebugSymbol elf_symbol;

void debug_init(ELFDebugSymbol symbol)
{
    printk("debug_init. \n");
    printk("symtab %p, size %x, strtab %p, size %x\n", symbol.symtab, symbol.symtabsz,  symbol.strtab, symbol.strtabsz);
    elf_symbol = symbol;
}

static const char *elf_lookup_symbol(uint64_t addr)
{
    for (int i = 0; i < (elf_symbol.symtabsz); i++)
    {
        if (ELF64_ST_TYPE(elf_symbol.symtab[i].st_info) != 0x2)
        {
            continue;
        }

        uint64_t size = elf_symbol.symtab[i].st_size;
        uint64_t start = elf_symbol.symtab[i].st_value;
        uint64_t end = start + size;

        // 通过函数调用地址查到函数的名字
        if ((addr >= start) && (addr < end))
        {
            return (const char *)((uint8_t *)elf_symbol.strtab + elf_symbol.symtab[i].st_name);
        }
    }

    return "unknown symbol";
}

static void print_stack_trace()
{
    uint64_t *rbp, *rip;

    // load current rbp
    asm volatile("mov %%rbp, %0"
                 : "=r"(rbp));

    // we keep poping stack until reaching start_kernel_base
    for (int i = 0; i < 10 && *rbp != start_kernel_base; ++i)
    // while (*rbp != start_kernel_base)
    {
        printk("rbp at %p\n", rbp);
        // rip is above the rbp because
        // call instruction will push rip and
        // the function will exec
        // push rbp;
        // mov rsp, rbp;
        rip = rbp + 1;
        // rip points to the stack which it's value *rip is the return address
        // the symbol in elf is mapped at the start of 0x1500
        // but we've shifted the address to kernel space
        // and we now executing with virtual address like 0xffff800000001500
        // so translation from *rip to it's original is needed
        auto symbol = elf_lookup_symbol(*rip);
        printk("   [%x] %s\n", *rip, symbol);

        // rbp points to the current position of rbp on stack
        // it's value *rbp points to the previous one on stack
        // we make rbp points to the previous one on stack
        rbp = (uint64_t *)*rbp;
    }
    
}

void panic(const char *msg)
{
    asm volatile("cli");
    printk("*** System panic: %s\n", msg);
    print_stack_trace();
    printk("***\n");

    // 致命错误发生后打印栈信息后停止在这里
    while (1)
        ;
}
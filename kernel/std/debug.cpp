#include "debug.h"
#include "elf/elf-em.h"
#include "printk.h"
#include <memory/physical_page.h>

// the rbp when we jump to kernel_start

extern char STACK_START;

static ELFDebugSymbol elf_symbol;

void debug_init(ELFDebugSymbol symbol)
{
    printk("debug_init. \n");
    elf_symbol = symbol;
}

static const char *elf_lookup_symbol(uint64_t addr)
{
    for (int i = 0; i < (elf_symbol.symtabsz); i++)
    {
        if (ELF64_ST_TYPE(elf_symbol.symtab[i].st_info) != STT_FUNC)
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

    auto kernel_stack_start = (uint64_t)&STACK_START;
    // we keep poping stack until reaching start_kernel_base
    for (int i = 0; i < 10 && *rbp != kernel_stack_start; ++i)
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

    while (1)
    {
        asm volatile(
            "cli \n\t"
            "hlt \n\t");
    };
}

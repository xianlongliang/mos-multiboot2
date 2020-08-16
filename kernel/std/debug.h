#pragma once

#include <std/stdint.h>
#include <std/elf/elf.h>

struct ELFDebugSymbol {
	Elf64_Sym *symtab;
	uint32_t symtabsz;
	
	const char *strtab;
	uint32_t strtabsz;
};

extern "C"
{
    void debug_init(ELFDebugSymbol symbol);
    void panic(const char *msg);
}

// 编译期间静态检测
#define static_assert(x) \
    switch (x)           \
    {                    \
    case 0:              \
    case (x):;           \
    }


#define assert(x) \
    do {    \
        if (x == false) panic(#x); \
    } while(0);

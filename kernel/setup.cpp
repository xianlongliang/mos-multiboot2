#include <std/stdint.h>
#include "setup.h"
#include <memory/physical.h>
#include <arch/x86_64/kernel.h>
#include <std/printk.h>
#include <std/elf/elf.h>
#include <std/debug.h>
#include <memory/virtual_page.h>

extern "C" char pml4;
extern "C" char pdpe;
extern "C" char pde;

static __attribute__((aligned(0x1000))) Page_PTE pte2_8[512 * 3];

void do_8mb_mapping() {

    printk("pml4 at %p, pdpe at %p, pde at %p\n", &pml4, &pdpe, &pde);
    auto pte_base = (Page_PTE *)(pte2_8);
    // one pde can cover up to 1GB area
    auto pde_base = (Page_PDE *)(Phy_To_Virt(&pde));
    // from pte_base we are going to build 3 entire pte from 0xd000 to 0xffff
    // each pte takes 0x1000 bytes
    bzero(pte_base, 0x1000 * 3);
    auto physical_base = 0x200000;
    auto step = 0x1000;

    for (int i = 1; i <= 3; ++i)
    {
        pde_base[i] = pde_base[0];
        // must be physical address
        pde_base[i].NEXT = uint64_t(Virt_To_Phy(pte_base)) >> 12;
        for (int j = 0; j < 512; ++j)
        {
            pte_base[j].P = 1;
            pte_base[j].R_W = 1;
            pte_base[j].U_S = 1;
            pte_base[j].PPBA = (physical_base * i + j * step) >> 12;
        }
        // switch to next pte base
        pte_base = (Page_PTE *)(uint64_t(pte_base) + 0x1000);
    }

    auto cr3 = Get_CR3();
    *(uint64_t *)cr3 = 0UL;
    flush_tlb();
}

void basic_init(void *mbi_addr)
{
    auto addr = mbi_addr;
    auto mbi_size = *(uint64_t *)addr;
    auto mbi_end = addr + mbi_size + KERNEL_VIRTUAL_START;
    PhysicalMemory::ZONE_VIRTUAL_START = (void *)mbi_end;

    for (auto tag = (struct multiboot_tag *)(addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
            multiboot_mmap_entry *mmap;

            printk("mmap entries count: %d\n", ((multiboot_tag_mmap *)tag)->entry_size);

            for (mmap = ((multiboot_tag_mmap *)tag)->entries;
                 (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                 mmap = (multiboot_mmap_entry *)((uint8_t *)mmap + ((multiboot_tag_mmap *)tag)->entry_size))
            {

                if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
                {
                    PhysicalMemory::GetInstance()->Add(mmap);
                }
            }
            break;
        }
        case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
        {
            auto elf_tag = (multiboot_tag_elf_sections *)tag;
            ELFDebugSymbol debug_symbol;
            for (int i = 0; i < elf_tag->num; ++i)
            {
                auto shdr = (Elf64_Shdr *)(elf_tag->sections + i * elf_tag->entsize);
                if (shdr->sh_type == SHT_SYMTAB)
                {
                    debug_symbol.symtab = (Elf64_Sym *)shdr->sh_addr;
                    debug_symbol.symtabsz = shdr->sh_size;
                }
                if (shdr->sh_type == SHT_STRTAB)
                {
                    debug_symbol.strtab = (const char *)shdr->sh_addr;
                    debug_symbol.strtabsz = shdr->sh_size;
                    break;
                }
            }
            debug_init(debug_symbol);
            break;
        }
        }
    }

    do_8mb_mapping();
}
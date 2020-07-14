#include <std/stdint.h>
#include "setup.h"
#include <memory/physical.h>
#include <arch/x86_64/kernel.h>
#include <std/printk.h>
#include <std/elf/elf.h>
#include <std/debug.h>

extern "C" uint8_t _kernel_virtual_start;

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
}
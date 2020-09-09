#pragma once

#include <std/stdint.h>
#include <multiboot2.h>
#include <memory/physical.h>
#include <std/debug.h>

#define MULTIBOOT_BOOTLOADER_NAME_SIZE 64
#define MULTIBOOT_COMMAND_LINE_SIZE 256
#define MULTIBOOT_MEMORY_MAP_SIZE 64
#define MULTIBOOT_MODULES_SIZE 16

struct MBI2
{
    inline static void *framebuffer_tag_address;
    inline static void *mmap_tag_address;
    inline static void *elf_section_tag_address;
    inline static void *module_tag_address;
    inline static void *acpi_new_tag_address;
    inline static void *acpi_old_tag_address;

    static void Init()
    {
        if (framebuffer_tag_address)
        {
            auto framebuffer_tag = (multiboot_tag_framebuffer *)framebuffer_tag_address;
            auto common = framebuffer_tag->common;
            GOP::Init(Phy_To_Virt(common.framebuffer_addr), common.framebuffer_width, common.framebuffer_height, common.framebuffer_pitch);
        }

        if (acpi_old_tag_address)
        {
            auto tag = (multiboot_tag *)acpi_old_tag_address;
            auto rsdp_tag = (multiboot_tag_old_acpi *)tag;
            printk("rsdp: %p, size: %d\n", rsdp_tag->rsdp, rsdp_tag->size);
            RSDP::GetInstance()->Init(1, (uint8_t *)&rsdp_tag->rsdp);
        }

        if (acpi_new_tag_address)
        {
            auto tag = (multiboot_tag *)acpi_old_tag_address;
            auto rsdp_tag = (multiboot_tag_new_acpi *)tag;
            printk("rsdp: %p, size: %d\n", rsdp_tag->rsdp, rsdp_tag->size);
            RSDP::GetInstance()->Init(2, (uint8_t *)&rsdp_tag->rsdp);
        }

        if (mmap_tag_address)
        {
            auto tag = (multiboot_tag *)mmap_tag_address;
            multiboot_mmap_entry *mmap;
            printk("mmap entries count: %d\n", ((multiboot_tag_mmap *)tag)->entry_size);
            auto pm = PhysicalMemory::GetInstance();
            uint64_t end;
            for (mmap = ((multiboot_tag_mmap *)tag)->entries;
                 (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                 mmap = (multiboot_mmap_entry *)((uint8_t *)mmap + ((multiboot_tag_mmap *)tag)->entry_size))
            {
                if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
                {
                    end = pm->Add(mmap);
                }
            }
            // auto reserved_page_count = (PAGE_4K_ROUND_UP(end) - (uint64_t)Phy_To_Virt(0x100000)) / PAGE_SIZE_4K;
            // printk("allocating memory ...\n");
            // for (int i = 0; i < reserved_page_count; ++i)
            // {
            //     pm->Allocate(1, 0);
            // }
            // printk("allocating memory done\n");
            // brk_done();
        }

        if (elf_section_tag_address)
        {
            auto tag = (multiboot_tag *)elf_section_tag_address;
            auto elf_tag = (multiboot_tag_elf_sections *)tag;
            ELFDebugSymbol debug_symbol;
            for (int i = 0; i < elf_tag->num; ++i)
            {
                auto shdr = (Elf64_Shdr *)(elf_tag->sections + i * elf_tag->entsize);
                if (shdr->sh_type == SHT_SYMTAB)
                {
                    debug_symbol.symtab = (Elf64_Sym *)Phy_To_Virt(shdr->sh_addr);
                    debug_symbol.symtabsz = shdr->sh_size;
                }
                if (shdr->sh_type == SHT_STRTAB)
                {
                    debug_symbol.strtab = (const char *)Phy_To_Virt(shdr->sh_addr);
                    debug_symbol.strtabsz = shdr->sh_size;
                    break;
                }
            }
            debug_init(debug_symbol);
        }

        if (module_tag_address)
        {
            auto tag = (multiboot_tag *)module_tag_address;
            auto module = (multiboot_tag_module *)tag;
            auto elf_header = (Elf64_Ehdr *)module->mod_start;
            printk("elf: %d\n", elf_header->e_shnum);
            printk("find module: %x, %x\n", module->mod_start, module->mod_end);
            printk("size: %d\n", module->size);
        }
    }
};
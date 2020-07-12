#include <multiboot2.h>
#include <std/stdint.h>
#include <std/printk.h>
#include <std/elf/elf.h>
#include <std/debug.h>
#include <std/new.h>
#include <memory/zone.h>
#include <memory/physical.h>

extern "C" uint8_t _kernel_virtual_end;
extern "C" uint8_t _kernel_virtual_start;

extern "C" void Kernel_Main(unsigned long addr)
{

  clear();

  auto mbi_size = *(uint64_t *)addr;
  printk("kernel virtual address from: %p to %p\n", &_kernel_virtual_start, addr + mbi_size);

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
          printk("base_addr = %x, length = %x, type = %d\n",
                 mmap->addr,
                 mmap->len,
                 mmap->type);
          PhysicalMemory::GetInstance()->Add(mmap);
        }
      }
      break;
    }
    case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
    {
      auto elf_tag = (multiboot_tag_elf_sections *)tag;
      ELFDebugSymbol debug_symbol;
      printk("elf section %p size: %d, num: %d, entsize: %d, shndx: %d\n", tag, elf_tag->size, elf_tag->num, elf_tag->entsize, elf_tag->shndx);
      for (int i = 0; i < elf_tag->num; ++i)
      {
        auto shdr = (Elf64_Shdr *)(elf_tag->sections + i * elf_tag->entsize);
        // printk("header %d, type %d, shdr %p\n", i, shdr->sh_type, shdr->sh_addr);
        if (shdr->sh_type == SHT_SYMTAB)
        {
          printk("SHT_SYMTAB addr: %p\n", shdr->sh_addr);
          debug_symbol.symtab = (Elf64_Sym *)shdr->sh_addr;
          debug_symbol.symtabsz = shdr->sh_size;
        }
        if (shdr->sh_type == SHT_STRTAB)
        {
          printk("SHT_STRTAB\n");
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
  while (1)
    ;
}

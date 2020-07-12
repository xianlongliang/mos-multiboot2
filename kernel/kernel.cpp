#include <multiboot2.h>
#include <std/stdint.h>
#include <std/printk.h>
#include <std/elf/elf.h>
#include <std/debug.h>

extern "C" void Kernel_Main(unsigned long addr)
{
  clear();
  auto size = *(uint64_t *)addr;
  printk("Announced mbi size %d at %x\n", size, addr);

  for (auto tag = (multiboot_tag *)(addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
  {
    switch (tag->type)
    {
    case MULTIBOOT_TAG_TYPE_MMAP:
    {
      multiboot_memory_map_t *mmap;

      printk("mmap entries: %d\n", ((multiboot_tag_mmap *)tag)->size);

      for (mmap = ((multiboot_tag_mmap *)tag)->entries;
           (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
           mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((multiboot_tag_mmap *)tag)->entry_size))
      {
        printk("base_addr = %x, length = %d, type = %d\n",
               mmap->addr,
               mmap->len,
               mmap->type);
      }
      break;
    }
    case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
    {
      auto elf_tag = (multiboot_tag_elf_sections *)tag;
      ELFDebugSymbol debug_symbol;
      printk("elf section size: %d, num: %d, entsize: %d, shndx: %d\n", elf_tag->size, elf_tag->num, elf_tag->entsize, elf_tag->shndx);
      for (int i = 0; i < elf_tag->num; ++i)
      {
        auto shdr = (Elf64_Shdr *)(elf_tag->sections + i * elf_tag->entsize);
        printk("header %d, type %d, shdr %p\n", i, shdr->sh_type, shdr->sh_addr);
        if (shdr->sh_type == SHT_SYMTAB) {
          printk("SHT_SYMTAB\n");
          debug_symbol.symtab = (Elf64_Sym*)shdr->sh_addr;
          debug_symbol.symtabsz = shdr->sh_size;
        }
        if (shdr->sh_type == SHT_STRTAB) {
          printk("SHT_STRTAB\n");
          debug_symbol.strtab = (const char*)shdr->sh_addr;
          debug_symbol.strtabsz = shdr->sh_size;
          break;
        }
      }
      debug_init(debug_symbol);
      panic("test");
      break;
    }
    }
  }
  while (1)
    ;
}

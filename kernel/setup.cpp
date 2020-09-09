#include <std/stdint.h>
#include "setup.h"
#include <memory/physical.h>
#include <arch/x86_64/kernel.h>
#include <std/printk.h>
#include <std/elf/elf.h>
#include <std/debug.h>
#include <memory/virtual_page.h>
#include <memory/mapping.h>
#include <acpi/rsdp.h>
#include <memory/heap.h>
#include <std/math.h>
#include <display/gop.h>
#include <multiboot2/mbi2.h>

extern "C" char pml4;
extern "C" char pdpe;
extern "C" char pde;

extern char _kernel_virtual_end;

static __attribute__((aligned(0x1000))) Page_PTE pte2_8[512 * 3];

void do_1gb_mapping()
{
  auto pdpe_base = ((Page_PDPE *)&pdpe);
  pdpe_base[0x1fd].P = 1;
  pdpe_base[0x1fd].R_W = 1;
  pdpe_base[0x1fd].SIZE = 1;
  pdpe_base[0x1fd].NEXT = (0x40000000) >> PAGE_4K_SHIFT;

  pdpe_base[0x1fe].P = 1;
  pdpe_base[0x1fe].R_W = 1;
  pdpe_base[0x1fe].SIZE = 1;
  pdpe_base[0x1fe].NEXT = (0x80000000) >> PAGE_4K_SHIFT;

  pdpe_base[0x1ff].P = 1;
  pdpe_base[0x1ff].R_W = 1;
  pdpe_base[0x1ff].SIZE = 1;
  pdpe_base[0x1ff].NEXT = (0xc0000000) >> PAGE_4K_SHIFT;

  flush_tlb();
}

void do_8mb_mapping()
{

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
      pte_base[j].PPBA = (physical_base * i + j * step) >> 12;
    }
    // switch to next pte base
    pte_base = (Page_PTE *)(uint64_t(pte_base) + 0x1000);
  }
}

void basic_init(void *mbi_addr)
{

  do_1gb_mapping();
  vmap_init();

  // /* render one glyph for UNICODE code point 'A', directly to the screen and then adjust pen position */
  // ssfn_putc(0x42);
  // ssfn_putc(0x43);

  mbi_addr = Phy_To_Virt(mbi_addr);

  auto addr = mbi_addr;
  auto mbi_size = *(unsigned *)addr;
  auto mbi_end = (uint8_t *)addr + mbi_size;
  auto free_vstart = (uint8_t *)PAGE_4K_ROUND_UP((uint64_t)mbi_end);
  auto free_vstart_2 = (uint8_t *)PAGE_4K_ROUND_UP((uint64_t)&_kernel_virtual_end);
  brk_init(max(free_vstart, free_vstart_2));
  for (auto tag = (struct multiboot_tag *)((uint8_t *)addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
  {
    switch (tag->type)
    {
    case MULTIBOOT_TAG_TYPE_EFI_BS:
    {
      break;
    }
    case MULTIBOOT_TAG_TYPE_EFI_MMAP:
    {
      auto efi_tag = (multiboot_tag_efi_mmap *)tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_VBE:
    {
      auto vbe_tag = (multiboot_tag_vbe *)tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_EFI64:
    {
      auto efi_tag = (multiboot_tag_efi64 *)tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
    {
      MBI2::framebuffer_tag_address = tag;
      break;
    }
    case MULTIBOOT_HEADER_TAG_ADDRESS:
    {
      auto address_tag = (multiboot_header_tag_address *)tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_ACPI_NEW:
    {
      MBI2::acpi_new_tag_address = (void *)tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_ACPI_OLD:
    {
      MBI2::acpi_old_tag_address = (void *)tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
    {
      auto rsdp_tag = (multiboot_tag_load_base_addr *)tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_MODULE:
    {
      MBI2::module_tag_address = tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_MMAP:
    {
      MBI2::mmap_tag_address = tag;
      break;
    }
    case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
    {
      MBI2::elf_section_tag_address = tag;
      break;
    }
    }
  }
  MBI2::Init();
}
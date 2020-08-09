OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

SECTIONS
{
  . = 0x0;
  _kernel_physical_start = .;
  .boot :
  {
    *(.multiboot_header)
  }
  . = ALIGN(0x1000);
  .page :
  {
    *(.page)
  }
  . = ALIGN(0x1000);
  .smp :
  {
    *(.smp)
  }
  .boot_text :
  {
    *(.boot_text)
  }
  . = ALIGN(0x1000);
  _boot_end = .;

  . = 0x100000;
  _kernel_physical_start = .;

  . += 0xFFFFFFFF00000000;
  _kernel_virtual_start = .;

  . = ALIGN(0x1000);
  .data : AT(_kernel_physical_start)
	{
		_kernel_data_start = .;
		*(.data)
		_kernel_data_end = .;
	}

  . = ALIGN(0x1000);
  .text :
  {
    *(.multiboot)
    *(.text)
  }

  . = ALIGN(0x1000);
  .rodata : 
	{
		*(.rodata)
	}
  
  . = ALIGN(0x1000);
  .bss :
	{
		_kernel_bss_start = .;
		*(.bss)
		*(COMMON)
		_kernel_bss_end = .;
	}

	_kernel_virtual_end = .;
}

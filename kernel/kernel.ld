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
  _boot_end = .;

  . += 0xFFFFFFFF80000000;
  _kernel_virtual_start = .;
  .text : AT(_boot_end)
  {
    *(.multiboot)
    *(.text)
  }

  . = ALIGN(0x1000);
  .rodata : 
	{
		_kernel_rodata_start = .;
		*(.rodata)
		_kernel_rodata_end = .;
	}

  . = ALIGN(0x1000);
  .data :
	{
		_kernel_data_start = .;
		*(.data)
		_kernel_data_end = .;
	}

  . = ALIGN(0x1000);

  .bss :
	{
		_kernel_bss_start = .;
		*(.bss)
		*(COMMON)
		_kernel_bss_end = .;
	}

	_kernel_end = .;
}

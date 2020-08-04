#include <std/printk.h>
#include <gdt.h>
#include <interrupt/idt.h>
#include <setup.h>
#include <thread/task.h>
#include "syscall.h"
#include <std/interrupt.h>
#include <memory/kmalloc.h>
#include <memory/slab.h>
#include <std/unique_ptr.h>
#include <std/list.h>
#include <std/vector.h>
#include <interrupt/io_apic.h>
#include <interrupt/apic.h>
#include <acpi/rsdt.h>
#include <acpi/rsdp.h>

extern "C" void Kernel_Main(unsigned long addr)
{
  clear();
  basic_init((void *)addr);
  gdt_init();
  idt_init();
  syscall_init();
  RSDT::GetInstance()->Init();
  APIC::GetInstance()->Init();
  kmalloc_init();
  task_init();
  // sti();

  while (1)
  {
    hlt();
  }
}

#include <std/printk.h>
#include <gdt.h>
#include <interrupt/idt.h>
#include <interrupt/timer.h>
#include <setup.h>

extern "C" void Kernel_Main(unsigned long addr)
{
  clear();
  basic_init((void*)addr);
  gdt_init();
  idt_init();
  timer_init(10000);

  while (1)
    ;
}

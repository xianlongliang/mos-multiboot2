#include <std/printk.h>
#include <gdt.h>
#include <interrupt/idt.h>
#include <interrupt/timer.h>
#include <setup.h>
#include <task.h>

extern "C" void Kernel_Main(unsigned long addr)
{
  clear();
  basic_init((void*)addr);
  gdt_init();
  idt_init();
  timer_init(10000);
  task_init();
  while (1)
    ;
}

#include <std/printk.h>
#include <gdt.h>
#include <interrupt/idt.h>
#include <interrupt/timer.h>
#include <setup.h>
#include <thread/task.h>
#include "syscall.h"
#include <std/interrupt.h>
#include <memory/kmalloc.h>

extern "C" void Kernel_Main(unsigned long addr)
{
  clear();
  basic_init((void *)addr);
  gdt_init();
  idt_init();
  syscall_init();
  kmalloc_init();
  timer_init(1);
  task_init();
  // sti();

  while (1)
  {
    hlt();
  }
}

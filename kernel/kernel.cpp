#include <std/printk.h>
#include <gdt.h>
#include <interrupt/idt.h>
#include <interrupt/timer.h>
#include <setup.h>
#include <thread/task.h>
#include <syscall.h>

extern "C" void Kernel_Main(unsigned long addr)
{
  clear();
  basic_init((void*)addr);
  gdt_init();
  idt_init();
  syscall_init();
  asm volatile("sti");
  // timer_init(100);
  // task_init();
  while (1)
    ;
}

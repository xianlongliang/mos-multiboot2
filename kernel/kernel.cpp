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
#include <smp/smp.h>
#include <std/map.h>
#include <std/math.h>
#include <std/spinlock.h>
#include <std/string.h>
#include <std/unordered_set.h>

extern "C" void Kernel_Main(void *mbi_addr)
{
  clear();
  basic_init(mbi_addr);
  kmalloc_init();
  RSDT::GetInstance()->Init();
  SMP::GetInstance()->Init();
  unordered_set<int> uset;
  int key = 1;
  uset.insert(1);
  uset.insert(2);
  uset.insert(3);
  uset.insert(4);
  uset.insert(5);
  uset.insert(6);
  uset.insert(7);
  uset.insert(8);
  uset.insert(9);
  uset.insert(10);
  uset.insert(11);
  uset.insert(12);
  uset.insert(7);
  uset.find(key);
  task_init();
}

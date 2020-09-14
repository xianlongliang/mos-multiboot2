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

class SP
{
public:
  SP() {}
};

extern "C" void Kernel_Main(void *mbi_addr)
{
  basic_init(mbi_addr);
  RSDT::GetInstance()->Init();
  kmalloc_init();
  // auto s = shared_ptr<UniqueTest>(new UniqueTest());
  SMP::GetInstance()->Init();
  task_init();
}

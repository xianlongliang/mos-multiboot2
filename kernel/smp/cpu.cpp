#include "cpu.h"
#include <memory/physical_page.h>

CPU::CPU() : cpus(PAGE_SIZE_4K / sizeof(cpu_struct)) {

}

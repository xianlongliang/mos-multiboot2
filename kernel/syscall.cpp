#include "syscall.h"
#include <std/msr.h>
#include <std/stdint.h>

void syscall_init() {
    wrmsr(MSR_STAR, ((uint64_t)0x0020) << 48);
}
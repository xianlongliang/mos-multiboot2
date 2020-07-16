#include "userland.h"

void userland_entry() {
    while(1) {
        asm volatile("movq $0x2, %rdi");
        asm volatile("syscall");
    }
}
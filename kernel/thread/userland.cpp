#include "userland.h"

void userland_entry() {
    while(1) {
        asm volatile("subq $0x10, %rsp");
        asm volatile("movq $0x2, %rdi");
        asm volatile("syscall");
        asm volatile("addq $0x10, %rsp");
    }
}
#pragma once 
#include <std/stdint.h>

void keyboard_irq_handler(uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip);

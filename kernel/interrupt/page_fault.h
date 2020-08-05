#pragma once

#include <std/stdint.h>

void page_fault_handler(uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip);

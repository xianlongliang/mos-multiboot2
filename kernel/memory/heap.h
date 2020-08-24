#pragma once

#include <std/stdint.h>

void *brk_get();
void *brk_up(uint64_t size, uint64_t alignment = 16);
void *brk_down(uint64_t size);
void brk_done();
void brk_init(void *addr);
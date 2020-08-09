#pragma once

#include <std/stdint.h>

void *brk_get();
void *brk_up(uint64_t size);
void *brk_down(uint64_t size);
void *heap_init(void *addr);
#pragma once
#include <std/stdint.h>

extern "C"
{
    void kmalloc_init();
    void *kmalloc(uint64_t size, uint64_t flags);
    void kfree(const void *ptr);
}
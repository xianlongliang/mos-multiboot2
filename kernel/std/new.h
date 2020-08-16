#pragma once
#include "stdint.h"
#include <memory/kmalloc.h>

void *operator new(size_t count);
void *operator new[](size_t count);

void operator delete(void *ptr, size_t sz);
void operator delete[](void *ptr, size_t sz);

// placement new
void *operator new(size_t count, void *ptr);
void *operator new[](size_t count, void *ptr);

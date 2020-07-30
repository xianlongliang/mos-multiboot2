#pragma once
#include "stdint.h"
#include <memory/kmalloc.h>

inline void *operator new(size_t size) { return kmalloc(size, 0); }
inline void operator delete(void *p, size_t size) { kfree(p); }
// placement new
inline void *operator new(size_t, void *p) { return p; }
inline void *operator new[](size_t size, void *p) { return kmalloc(size, 0); }
inline void operator delete(void *, void *){};
inline void operator delete[](void *p, void *) { kfree(p); };
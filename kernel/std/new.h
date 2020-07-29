#pragma once
#include "stdint.h"
#include <memory/kmalloc.h>

inline void *operator new(size_t size) { return kmalloc(size, 0); }
inline void operator delete(void *p, size_t size) { kfree(p); }
inline void *operator new(size_t, void *p) throw() { return p; }
inline void *operator new[](size_t, void *p) throw() { return p; }
inline void operator delete(void *, void *)throw(){};
inline void operator delete[](void *, void *) throw(){};
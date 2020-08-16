#include "new.h"

// replaceable allocation functions
void *operator new(size_t count)
{
    return kmalloc(count, 0);
}

void *operator new[](size_t count)
{
    return kmalloc(count, 0);
}

void *operator new(size_t count, void *ptr)
{
    return ptr;
}

void *operator new[](size_t count, void *ptr)
{
    return ptr;
}

void operator delete(void *ptr, size_t sz)
{
    kfree(ptr);
}

void operator delete[](void *ptr, size_t sz)
{
    kfree(ptr);
}
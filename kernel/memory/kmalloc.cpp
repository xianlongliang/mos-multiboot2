#include "kmalloc.h"
#include "lrmalloc/lrmalloc.h"
#include <std/printk.h>

void kmalloc_init()
{
    lrmalloc_init();
}

void *kmalloc(uint64_t size, uint64_t flags)
{
    return lrmalloc(size);
}

void kfree(const void *ptr)
{
    // printk("free %p\n", ptr);
    lrfree(ptr);
}

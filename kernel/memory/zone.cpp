#include "zone.h"
#include <std/debug.h>
#include <std/printk.h>
#include <multiboot2.h>
#include <arch/x86_64/kernel.h>
#include "flags.h"

uint64_t round_up_pow_of_2(uint64_t x) { return x == 1 ? 1 : 1 << (64 - __builtin_clzl(x - 1)); }
#define LEFT_LEAF(index) ((index)*2 + 1)
#define RIGHT_LEAF(index) ((index)*2 + 2)
#define PARENT(index) (((index) + 1) / 2 - 1)

#define IS_POWER_OF_2(x) (!((x) & ((x)-1)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

uint64_t Zone::PageSize()
{
    return this->total_pages_count * sizeof(Page);
}

Zone::Zone(multiboot_mmap_entry *mmap)
{
    this->physical_start_address = PAGE_2M_ALIGN(mmap->addr);
    this->physical_end_address = ((mmap->addr + mmap->len) >> PAGE_2M_SHIFT) << PAGE_2M_SHIFT;
    this->attribute = 0;

    uint64_t pages_count = (this->physical_end_address - this->physical_start_address) >> PAGE_2M_SHIFT;
    printk("page size: 2M, avaliable pages: %d\n", pages_count);

    this->free_pages_count = pages_count;
    this->total_pages_count = pages_count;

    if (!IS_POWER_OF_2(pages_count))
    {
        this->total_pages_count_rounded_up = round_up_pow_of_2(pages_count);
        printk("buddy page round %d\n", this->total_pages_count_rounded_up);
    }

    // node number is size * 2 - 1, we alloc size * 2
    auto node_size = this->total_pages_count_rounded_up * 2;
    // nodes placed at the end of the zone
    this->nodes = (uint64_t *)(uint64_t(this) + sizeof(Zone));
    // nodes placed at the end of the nodes
    this->pages = (Page *)(uint64_t(this) + sizeof(Zone) + node_size * sizeof(uint64_t));
    printk("nodes start at %p -> %p\n", nodes, uint64_t(this->nodes) + node_size * sizeof(uint64_t));
    printk("pages start at %p -> %p\n", pages, uint64_t(pages) + this->total_pages_count * sizeof(Page));

    // build the buddy tree
    for (int i = 0; i < 2 * this->total_pages_count_rounded_up - 1; ++i)
    {
        if (IS_POWER_OF_2(i + 1))
            node_size /= 2;
        this->nodes[i] = node_size;
    }
    // init each page
    for (int j = 0; j < this->FreePagesCount(); ++j)
    {
        pages[j].zone = this;
        pages[j].physical_address = this->physical_start_address + PAGE_2M_SIZE * j;
        pages[j].attributes = 0;
        pages[j].reference_count = 0;
        pages[j].age = 0;
    }

    auto reserved_pages = (PAGE_2M_ALIGN(uint64_t(this) + this->Span()) - KERNEL_VIRTUAL_START) / PAGE_2M_SIZE;
    printk("reserved pages %d\n", reserved_pages);
    printk("zone init, start at: %p span: %x\n", this, this->Span());
    auto pidx = this->AllocatePages(reserved_pages);
    printk("reserved page %d start at %p\n", pidx, this->pages[pidx].physical_address);
    // pages[pidx].attributes = ...
}

int64_t Zone::AllocatePages(uint64_t pages_count)
{
    assert(pages_count >= 1, "pages_count < 1");

    if (!IS_POWER_OF_2(pages_count))
        pages_count = round_up_pow_of_2(pages_count);
    unsigned index = 0;
    // if the first(largest) doesn't fit return error
    if (this->nodes[index] < pages_count) {
        printk("this->nodes[0] == %d < %d\n", this->nodes[0], pages_count);
        return -1;
    }
        

    auto node_size = this->total_pages_count_rounded_up;
    for (; node_size != pages_count; node_size /= 2)
    {
        if (this->nodes[LEFT_LEAF(index)] >= pages_count)
            index = LEFT_LEAF(index);
        else
            index = RIGHT_LEAF(index);
    }
    auto offset = (index + 1) * node_size - this->total_pages_count_rounded_up;
    if (offset > this->total_pages_count)
    {
        return -1;
    }

    // 标记为已用
    this->nodes[index] = 0;

    while (index)
    {
        index = PARENT(index);
        this->nodes[index] =
            MAX(this->nodes[LEFT_LEAF(index)], this->nodes[RIGHT_LEAF(index)]);
    }

    return offset;
}

int64_t Zone::FreePages(uint64_t offset)
{
    unsigned node_size, index = 0;
    unsigned left_longest, right_longest;

    assert(offset >= 0 && offset < this->total_pages_count, "self && offset >= 0 && offset < self->size");

    node_size = 1;
    index = offset + this->total_pages_count - 1;

    for (; this->nodes[index]; index = PARENT(index))
    {
        node_size *= 2;
        if (index == 0)
            return 0;
    }

    this->nodes[index] = node_size;

    while (index)
    {
        index = PARENT(index);
        node_size *= 2;

        left_longest = this->nodes[LEFT_LEAF(index)];
        right_longest = this->nodes[RIGHT_LEAF(index)];

        if (left_longest + right_longest == node_size)
            this->nodes[index] = node_size;
        else
            this->nodes[index] = MAX(left_longest, right_longest);
    }
    return 1;
}

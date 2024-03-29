#include "zone.h"
#include <std/debug.h>
#include <std/printk.h>
#include <multiboot2.h>
#include <arch/x86_64/kernel.h>
#include "physical_page.h"
#include "flags.h"
#include <std/math.h>
#include <memory/heap.h>
#include <std/lock_guard.h>
#include <memory/physical.h>

#define LEFT_LEAF(index) ((index)*2 + 1)
#define RIGHT_LEAF(index) ((index)*2 + 2)
#define PARENT(index) (((index) + 1) / 2 - 1)

#define IS_POWER_OF_2(x) (!((x) & ((x)-1)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

uint64_t Zone::PageSize()
{
    return this->total_pages_count * sizeof(Page);
}

Zone::Zone(uint8_t *pstart, uint8_t *pend)
{

    list_init(&this->list_node);

    this->physical_start_address = (uint64_t)pstart;
    this->physical_end_address = (uint64_t)pend;
    this->attribute = 0;

    uint64_t pages_count = (this->physical_end_address - this->physical_start_address) / PAGE_4K_SIZE;
    printk("page size: 4k, avaliable pages: %d\n", pages_count);
    this->free_pages_count = pages_count;
    this->total_pages_count = pages_count;
    this->total_pages_count_rounded_up = pages_count;

    if (!IS_POWER_OF_2(pages_count))
    {
        this->total_pages_count_rounded_up = next_pow_of_2(pages_count);
        printk("buddy page round %d\n", this->total_pages_count_rounded_up);
    }

    // node number is size * 2 - 1, we alloc size * 2
    auto node_size = this->total_pages_count_rounded_up * 2;
    // nodes placed at the end of the zone
    auto zone_end = (int8_t *)this + sizeof(Zone);
    this->nodes = (uint32_t *)zone_end;
    auto nodes_end = zone_end + sizeof(this->nodes) * node_size;
    // nodes placed at the end of the nodes
    this->pages = (Page *)nodes_end;
    auto pages_end = nodes_end + sizeof(Page) * this->total_pages_count;

    printk("zone start at %p -> %p\n", this, uint64_t(this) + sizeof(Zone));
    printk("nodes start at %p -> %p\n", nodes, uint64_t(this->nodes) + node_size * sizeof(uint32_t));
    printk("pages start at %p -> %p\n", pages, uint64_t(pages) + this->total_pages_count * sizeof(Page));
    this->zone_end = uint64_t(pages) + this->total_pages_count * sizeof(Page);
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
        pages[j].physical_address = (uint8_t *)(this->physical_start_address + PAGE_4K_SIZE * j);
        pages[j].attributes = 0;
        pages[j].reference_count = 0;
    }

    auto reserved_page_count = (PAGE_4K_ROUND_UP((uint64_t)pages_end) - (uint64_t)Phy_To_Virt(pstart)) / PAGE_4K_SIZE;
    printk("reserving memory ...\n");
    for (int i = 0; i < reserved_page_count; ++i)
    {
        this->AllocatePages(1);
    }
    printk("reserving memory done\n");
}

int64_t Zone::AllocatePages(uint64_t pages_count)
{
    LockGuard<Spinlock> lg(this->lock);
    assert(pages_count >= 1);

    if (!IS_POWER_OF_2(pages_count))
        pages_count = next_pow_of_2(pages_count);
    unsigned index = 0;
    // if the first(largest) doesn't fit return error
    if (this->nodes[index] < pages_count)
    {
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
bool Zone::Reserve(uint64_t page_offset)
{
    LockGuard<Spinlock> lg(this->lock);
    if (page_offset > this->total_pages_count)
        return true;
    // make sure the branch is free
    auto page_index = ((page_offset + this->total_pages_count_rounded_up) / 1) - 1;
    auto index = page_index;
    do
    {
        if (this->nodes[index] == 0)
            return false;
        index = PARENT(index);
    } while (index > 0);

    this->nodes[page_index] = 0;
    return true;
}

int64_t Zone::FreePages(uint64_t offset)
{
    LockGuard<Spinlock> lg(this->lock);
    unsigned node_size, index = 0;
    unsigned left_longest, right_longest;

    assert(offset >= 0 && offset < this->total_pages_count);

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

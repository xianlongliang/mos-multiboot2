#include "slab.h"
#include "kmalloc.h"
#include "physical.h"

#include <std/printk.h>
#include <std/kstring.h>
#include <std/debug.h>

#include <std/math.h>

static SlabNode *slab_node_create(uint64_t object_size)
{
    auto slab_node = (SlabNode *)kmalloc(sizeof(SlabNode), 0);
    if (!slab_node)
    {
        printk("!slab_node\n");
        return nullptr;
    }
    bzero(slab_node, sizeof(SlabNode));

    // slab_node->page = PhysicalMemory::GetInstance()->Allocate(1, 0);

    // if (!slab_node->page)
    // {
    //     printk("!slab_node->page\n");
    //     kfree(slab_node);
    //     return nullptr;
    // }

    slab_node->free_count = PAGE_4K_SIZE / object_size;
    slab_node->bitmap_size = slab_node->free_count / 8 + 1;
    slab_node->bitmap = (uint8_t *)kmalloc(slab_node->bitmap_size / 8, 0);
    if (!slab_node->bitmap)
    {
        printk("!slab_node->bitmap\n");
        kfree(slab_node);
        // PhysicalMemory::GetInstance()->Free(slab_node->page);
        return nullptr;
    }

    bzero(slab_node->bitmap, slab_node->bitmap_size);
}

Slab *slab_create(uint64_t object_size)
{
    object_size = ROUND_UP_8BYTES(object_size);

    auto slab = (Slab *)kmalloc(sizeof(Slab), 0);
    if (!slab)
    {
        printk("!slab\n");
        return nullptr;
    }
    bzero(slab, sizeof(Slab));

    auto slab_node = slab_node_create(object_size);
    if (!slab_node)
    {
        printk("!slab_node\n");
        kfree(slab);
        return nullptr;
    }

    list_init(&slab_node->list);

    slab->object_size = object_size;
    slab->total_used = 0;
    slab->total_free = slab_node->free_count;
    slab->pool = slab_node;
}

int slab_free(Slab *slab)
{
    if (slab->total_used != 0)
    {
        printk("slab->total_used != 0\n");
        return -1;
    }
}

void *Slab::Alloc()
{
    SlabNode *selected_node = nullptr;
    if (this->total_free == 0)
    {
        auto slab_node = slab_node_create(this->object_size);
        if (!slab_node)
        {
            printk("!slab_node\n");
            return nullptr;
        }
        this->total_free += slab_node->free_count;
        list_add_to_behind(&this->pool->list, &slab_node->list);
        selected_node = slab_node;
    }
    else
    {
        selected_node = this->pool;
        do
        {
            // if the current node is full, check next
            if (selected_node->free_count == 0)
            {
                selected_node = container_of(list_next(&selected_node->list), struct SlabNode, list);
                continue;
            }

            // scan the bitmap
            for (int i = 0; i < selected_node->bitmap_size; ++i)
            {
                // if found
                if (1)
                {
                    selected_node->free_count--;
                    selected_node->used_count++;
                    this->total_free--;
                    this->total_used++;
                    return (void *)selected_node->vaddr + this->object_size * i;
                }
                panic("selected_node bit not found\n");
            }
        } while (selected_node);
    }
}

void Slab::Free(const void *ptr)
{
}
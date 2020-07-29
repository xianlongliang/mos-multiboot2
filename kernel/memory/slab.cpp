#include "slab.h"
#include "kmalloc.h"
#include "physical.h"

#include <std/printk.h>
#include <std/kstring.h>
#include <std/debug.h>
#include <std/new.h>
#include <std/math.h>

static SlabNode *slab_node_create(uint64_t object_size)
{
    auto slab_node = new SlabNode();
    if (!slab_node)
    {
        printk("!slab_node\n");
        return nullptr;
    }
    bzero(slab_node, sizeof(SlabNode));

    slab_node->free_count = PAGE_4K_SIZE / object_size == 0 ? 1 : PAGE_4K_SIZE / object_size;
    auto bitmap_size = (slab_node->free_count / 8) == 0 ? 1 : (slab_node->free_count / 8);
    slab_node->bitmap2 = new Bitmap(bitmap_size);
    if (!slab_node->bitmap2)
    {
        printk("!slab_node->bitmap\n");
        kfree(slab_node);
        return nullptr;
    }

    slab_node->vaddr = (uint8_t *)kmalloc(PAGE_4K_SIZE, 0);

    if (!slab_node->vaddr)
    {
        printk("!slab_node->vaddr\n");
        delete slab_node->bitmap2;
        kfree(slab_node);
        return nullptr;
    }

    return slab_node;
}

Slab *slab_create(uint64_t object_size)
{
    object_size = ROUND_UP_8BYTES(object_size);

    auto slab = new Slab();
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
    slab_node->slab = slab;

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
    SlabNode *selected_node = this->pool;

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

    do
    {
        // if the current node is full, check next
        if (selected_node->free_count == 0)
        {
            selected_node = container_of(list_next(&selected_node->list), struct SlabNode, list);
            continue;
        }

        for (int i = 0; i < selected_node->bitmap2->Size(); ++i)
        {

            if (!selected_node->bitmap2->IsSet(i))
            {
                selected_node->free_count--;
                selected_node->used_count++;
                this->total_free--;
                this->total_used++;
                selected_node->bitmap2->Set(i);
                return (void *)selected_node->vaddr + this->object_size * i;
            }
            panic("selected_node bit not found\n");
        }

    } while (selected_node != this->pool);
}

void Slab::Free(const void *ptr)
{
    auto selected_node = this->pool;
    do
    {
        auto vstart = selected_node->vaddr;
        auto vend = vstart + this->object_size * (selected_node->used_count + selected_node->free_count);

        if (vstart <= ptr && ptr < vend)
        {
            auto bit_offset = ((int8_t *)ptr - (int8_t *)vstart) / this->object_size;
            selected_node->bitmap2->UnSet(bit_offset);
            selected_node->free_count++;
            selected_node->used_count--;
            selected_node->slab->total_free++;
            selected_node->slab->total_used--;
            return;
        }

    } while (selected_node != this->pool);
}
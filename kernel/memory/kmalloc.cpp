#include "kmalloc.h"
#include "slab.h"
#include <std/math.h>
#include "physical.h"
#include <std/debug.h>

struct kmalloc_meta
{
    SlabNode *slab_node;
};

static void *brk = (void *)0x800000 + PAGE_OFFSET;

static void *brk_alloc(uint64_t size)
{
    printk("brk_alloc %d\n", size);
    auto res = brk;
    brk = brk + size;
    return res;
}
#define KMALLOC_CACHE_COUNT 10

static Slab kmalloc_cache[KMALLOC_CACHE_COUNT];

void kmalloc_init()
{
    int init_slab_size = 32;
    for (int i = 0; i < 10; ++i, init_slab_size *= 2)
    {
        kmalloc_cache[i].object_size = init_slab_size;
        kmalloc_cache[i].total_used = 0;
        kmalloc_cache[i].total_free = PAGE_4K_SIZE / init_slab_size == 0 ? 1 : PAGE_4K_SIZE / init_slab_size;
        auto slab_node = (SlabNode *)brk_alloc(sizeof(SlabNode));
        list_init(&slab_node->list);
        slab_node->slab = &kmalloc_cache[i];
        slab_node->used_count = 0;
        slab_node->free_count = kmalloc_cache[i].total_free;
        slab_node->bitmap_size = (slab_node->free_count / 8) == 0 ? 1 : (slab_node->free_count / 8);
        slab_node->bitmap = (uint8_t *)brk_alloc(PAGE_4K_SIZE - sizeof(SlabNode));
        slab_node->vaddr = brk_alloc(init_slab_size < PAGE_4K_SIZE ? PAGE_4K_SIZE : init_slab_size);
        bzero(slab_node->bitmap, slab_node->bitmap_size);
        kmalloc_cache[i].pool = slab_node;
    }
}

void kmalloc_create(int idx)
{
    auto &cache = kmalloc_cache[idx];
    auto slab_node = (SlabNode *)brk_alloc(sizeof(SlabNode));

    slab_node->used_count = 0;
    slab_node->free_count = PAGE_4K_SIZE / cache.object_size;
    slab_node->vaddr = brk_alloc(PAGE_4K_SIZE);
    slab_node->bitmap_size = (slab_node->free_count / 8) == 0 ? 1 : (slab_node->free_count / 8);
    slab_node->bitmap = (uint8_t *)brk_alloc(slab_node->bitmap_size);
    bzero(slab_node->bitmap, slab_node->bitmap_size);
    list_init(&slab_node->list);
    list_add_to_behind(&cache.pool->list, &slab_node->list);
    cache.total_free += slab_node->free_count;
}

void *kmalloc(uint64_t size, uint64_t flags)
{
    size += sizeof(kmalloc_meta);
    if (size > pow(2, KMALLOC_CACHE_COUNT + 5) - 16)
    {
        return nullptr;
    }
    size = round_up_pow_of_2(size);
    int fit_index = 0;
    int fit_size = 32;
    while (fit_size != size)
    {
        fit_size *= 2;
        fit_index++;
    }

    auto &cache = kmalloc_cache[fit_index];
    auto selected_node = cache.pool;
    if (cache.total_free == 0)
    {
        kmalloc_create(fit_index);
    }
    do
    {
        if (selected_node->free_count == 0)
        {
            selected_node = container_of(list_next(&selected_node->list), struct SlabNode, list);
            continue;
        }

        for (int i = 0; i < selected_node->bitmap_size * sizeof(selected_node->bitmap); ++i)
        {
            for (int j = 0; j < sizeof(selected_node->bitmap); j++)
            {
                if (!CHECK_BIT(selected_node->bitmap[i], j))
                {
                    selected_node->free_count--;
                    selected_node->used_count++;
                    cache.total_free--;
                    cache.total_used++;
                    SET_BIT(selected_node->bitmap[i], j);
                    auto meta = (kmalloc_meta *)((void *)selected_node->vaddr + cache.object_size * (sizeof(selected_node->bitmap) * i + j));
                    meta->slab_node = selected_node;
                    return (void *)meta + 16;
                }
            }
        }
    } while (selected_node != cache.pool);
}

void kfree(const void *ptr)
{
    // ptr must align 16 byte
    auto meta = (kmalloc_meta *)(ptr - 16);
    auto slab_node = meta->slab_node;
    auto bit_offset = ((int8_t *)meta - (int8_t *)slab_node->vaddr) / slab_node->slab->object_size;
    auto bit_offset_idx = bit_offset / 8;
    auto bit_offset_mod = bit_offset % 8;
    UNSET_BIT(slab_node->bitmap[bit_offset_idx], bit_offset_mod);
    slab_node->free_count++;
    slab_node->used_count--;
    slab_node->slab->total_free++;
    slab_node->slab->total_used--;
}
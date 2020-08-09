#pragma once
#include <std/stdint.h>
#include <std/list.h>
#define PAGE_OFFSET 0xFFFFFFFF00000000
#define Virt_To_Phy(addr) ((uint8_t *)(addr)-PAGE_OFFSET)
#define Phy_To_Virt(addr) ((uint8_t *)((uint8_t *)(addr) + PAGE_OFFSET))

#define PAGE_1G_SHIFT 30
#define PAGE_2M_SHIFT 21
#define PAGE_4K_SHIFT 12

#define PAGE_2M_SIZE (1UL << PAGE_2M_SHIFT)
#define PAGE_4K_SIZE (1UL << PAGE_4K_SHIFT)

#define PAGE_2M_MASK_LOW (~(PAGE_2M_SIZE - 1))
#define PAGE_4K_MASK_LOW (~(PAGE_4K_SIZE - 1))

#define PAGE_2M_MASK_HIGH (~PAGE_2M_MASK_LOW)
#define PAGE_4K_MASK_HIGH (~PAGE_4K_MASK_LOW)

#define PAGE_2M_ALIGN(addr) (((unsigned long)(addr) + PAGE_2M_SIZE - 1) & PAGE_2M_MASK_LOW)
#define PAGE_4K_ALIGN(addr) (((unsigned long)(addr) + PAGE_4K_SIZE - 1) & PAGE_4K_MASK_LOW)

#define PAGE_4K_ROUND_UP(addr) (addr == (addr & PAGE_4K_MASK_LOW) ? addr : ((addr & PAGE_4K_MASK_LOW) + PAGE_4K_SIZE))
#define PAGE_2M_ROUND_UP(addr) (addr == (addr & PAGE_2M_MASK_LOW) ? addr : ((addr & PAGE_2M_MASK_LOW) + PAGE_2M_SIZE))

#define PAGE_4K_ROUND_DOWN(addr) (addr & PAGE_4K_MASK_LOW)
#define PAGE_2M_ROUND_DOWN(addr) (addr & PAGE_2M_MASK_LOW)

class Zone;
struct Page
{
    Zone *zone;
    void *physical_address;
    List list;
    uint16_t reference_count;
    uint16_t attributes;
};
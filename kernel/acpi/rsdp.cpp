#include "rsdp.h"
#include <memory/physical.h>
#include <memory/mapping.h>
#include <memory/heap.h>
#include <std/debug.h>

struct acpi_rsdp_t
{
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    char reserved[3];
} __attribute__((__packed__));

void *RSDP::RSDTAddress()
{
    return this->rsdt_vaddr;
}

void *RSDP::RSDTAddressBase()
{
    return this->rsdt_vaddr_base;
}

void RSDP::Init(uint8_t rsdp_version, void *rsdp_address)
{
    switch (rsdp_version)
    {
    case 0:
    {
        auto rsdp = (acpi_rsdp_t *)rsdp_address;
        printk("rsdt addr: %p\n", rsdp->rsdt_address);
        PhysicalMemory::GetInstance()->Reserve(rsdp->rsdt_address);
        this->rsdt_vaddr_base = brk_up(PAGE_4K_SIZE);
        vmap_frame_kernel(this->rsdt_vaddr_base, (void *)(rsdp->rsdt_address & PAGE_4K_MASK_LOW));
        this->rsdt_vaddr = (void *)((uint64_t)this->rsdt_vaddr_base + (rsdp->rsdt_address & PAGE_4K_MASK_HIGH));
        break;
    }

    default:
    {
        panic("unknown rsdp_version\n");
        break;
    }
    }
}

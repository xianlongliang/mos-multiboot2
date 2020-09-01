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
    // below for Version 2.0
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    char reserved[3];
} __attribute__ ((packed));

uint8_t*RSDP::RSDTAddress()
{
    return this->rsdt_vaddr;
}

uint8_t*RSDP::RSDTAddressBase()
{
    return this->rsdt_vaddr_base;
}

void RSDP::Init(uint8_t rsdp_version, uint8_t*rsdp_address)
{
    switch (rsdp_version)
    {
    case 1:
    {
        auto rsdp = (acpi_rsdp_t *)rsdp_address;
        printk("rsdt addr: %p\n", rsdp->rsdt_address);
        PhysicalMemory::GetInstance()->Reserve(rsdp->rsdt_address);
        this->rsdt_vaddr = Phy_To_Virt((uint8_t*)(rsdp->rsdt_address));
        this->acpi_version = 1;
        break;
    }
    case 2:
    {
        auto rsdp = (acpi_rsdp_t *)rsdp_address;
        printk("rsdt addr: %p\n", rsdp->rsdt_address);
        PhysicalMemory::GetInstance()->Reserve(rsdp->rsdt_address);
        this->rsdt_vaddr = Phy_To_Virt((uint8_t*)(rsdp->rsdt_address));
        this->acpi_version = 2;
        break;
    }
    default:
    {
        panic("unknown rsdp_version\n");
        break;
    }
    }
}

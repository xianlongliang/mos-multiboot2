#include "rsdt.h"
#include "rsdp.h"
#include <std/stdint.h>
#include <std/debug.h>
#include <std/kstring.h>
#include <memory/physical_page.h>
#include <interrupt/io_apic.h>
#include <smp/cpu.h>

struct acpi_rsdt_header_t
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

struct acpi_rsdt_t
{
    acpi_rsdt_header_t header;
    uint32_t other_sdt[0];
};

struct acpi_apic_t
{
    acpi_rsdt_header_t header;
    uint32_t local_apic_address;
    uint32_t flags;
    int8_t entry_start[0];
    struct entry
    {
        uint8_t type;
        uint8_t length;
    };
    struct processor_local_apic_entry
    {
        entry header;
        uint8_t apic_processor_id;
        uint8_t apic_id;
        uint32_t flags;
    };
    struct io_apic_entry
    {
        entry header;
        uint8_t io_apic_id;
        uint8_t reserved;
        uint32_t io_apic_address;
        uint32_t global_system_interrupt_base;
    };
    struct interrupt_source_override_entry
    {
        entry header;
        uint8_t bus_source;
        uint8_t irq_source;
        uint32_t global_system_interrupt;
        uint16_t flags;
    };
    struct non_maskable_interrupts_entry
    {
        entry header;
        uint8_t acpi_process_id;
        uint16_t flags;
        uint8_t lint;
    };
    struct local_apic_address_override_entry
    {
        entry header;
        uint16_t reserved;
        uint64_t address;
    };
};

static bool rsdt_checksum(acpi_rsdt_t *header)
{
    unsigned char sum = 0;

    for (int i = 0; i < header->header.length; i++)
    {
        sum += ((char *)header)[i];
    }

    return sum == 0;
}

void RSDT::Init()
{
    auto rsdp = RSDP::GetInstance();

    auto rsdt = (acpi_rsdt_t *)rsdp->RSDTAddress();
    if (!rsdt_checksum(rsdt))
    {
        panic("rsdt checksum error\n");
    }
    auto rsdt_addr = rsdp->RSDTAddress();
    auto entry_len = (rsdt->header.length - sizeof(acpi_rsdt_t)) / 4;
    for (int i = 0; i < entry_len; ++i)
    {
        auto other = (acpi_rsdt_t *)(Phy_To_Virt(rsdt->other_sdt[i]));
        // parse madt
        // check https://wiki.osdev.org/MADT
        if (!strncmp(other->header.signature, "APIC", 4))
        {
            printk("find APIC\n");
            auto apic_header = (acpi_apic_t *)other;
            auto end_addr = (int8_t *)other + other->header.length;
            auto start_addr = apic_header->entry_start;
            while (start_addr < end_addr)
            {
                auto ent = (acpi_apic_t::entry *)start_addr;
                // printk("type: %d, len: %d\n", ent->type, ent->length);
                switch (ent->type)
                {
                // Processor Local APIC
                case 0:
                {
                    auto plae = (acpi_apic_t::processor_local_apic_entry *)ent;
                    if (plae->flags == 0)
                    {
                        printk("cpu: %d disabled\n", plae->apic_processor_id);
                    }
                    else
                    {
                        CPU::GetInstance()->Add(plae->apic_id);
                    }

                    break;
                }
                // I/O APIC
                case 1:
                {
                    auto iae = (acpi_apic_t::io_apic_entry *)ent;
                    printk("io_apic id: %d, address %p\n", iae->io_apic_id, iae->io_apic_address);
                    IOAPIC::GetInstance()->Init(iae->io_apic_address, iae->io_apic_id, iae->global_system_interrupt_base);
                    break;
                }
                case 2:
                {
                    auto isoe = (acpi_apic_t::interrupt_source_override_entry *)ent;
                    break;
                }
                case 4:
                {
                    auto nmie = (acpi_apic_t::non_maskable_interrupts_entry *)ent;
                    break;
                }
                case 5:
                {
                    auto laaoe = (acpi_apic_t::local_apic_address_override_entry *)ent;
                    break;
                }
                default:
                    break;
                }
                start_addr += ent->length;
            }
        }
        if (!strncmp(other->header.signature, "FACP", 4))
        {
            printk("find\n");
        }
    }

}
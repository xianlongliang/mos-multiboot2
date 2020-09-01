#include "rsdt.h"
#include "rsdp.h"
#include <std/stdint.h>
#include <std/debug.h>
#include <std/kstring.h>
#include <memory/physical_page.h>
#include <interrupt/io_apic.h>
#include <smp/cpu.h>
#include <std/move.h>

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
    uint32_t other_sdt[];
    typedef uint32_t* entry_type;
};

struct acpi_xsdt_t
{
    acpi_rsdt_header_t header;
    uint32_t other_sdt[];
    typedef uint32_t* entry_type;
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

template <class SDT_TYPE>
static bool rsdt_checksum(SDT_TYPE *header)
{
    unsigned char sum = 0;

    for (int i = 0; i < header->header.length; i++)
    {
        sum += ((char *)header)[i];
    }

    return sum == 0;
}
template <class SDT_TYPE>
void do_init(RSDP *rsdp)
{
    auto rsdt = (SDT_TYPE *)rsdp->RSDTAddress();
    if (!rsdt_checksum(rsdt))
    {
        panic("rsdt checksum error\n");
    }
    auto rsdt_addr = rsdp->RSDTAddress();
    auto size = sizeof(decltype(*rsdt->other_sdt));
    auto entry_len = (rsdt->header.length - sizeof(rsdt->header)) / sizeof(decltype(*rsdt->other_sdt));
    for (uint64_t i = 0; i < entry_len; ++i)
    {
        auto other = (typename SDT_TYPE::entry_type)&rsdt->other_sdt;
        SDT_TYPE* entry = (SDT_TYPE*)other[i];

        entry = (SDT_TYPE*)Phy_To_Virt(entry);

        // parse madt
        // check https://wiki.osdev.org/MADT
        if (!strncmp(entry->header.signature, "APIC", 4))
        {
            printk("find APIC\n");
            auto apic_header = (acpi_apic_t *)entry;
            auto end_addr = (int8_t *)entry + entry->header.length;
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
                    // auto isoe = (acpi_apic_t::interrupt_source_override_entry *)ent;
                    break;
                }
                case 4:
                {
                    // auto nmie = (acpi_apic_t::non_maskable_interrupts_entry *)ent;
                    break;
                }
                case 5:
                {
                    // auto laaoe = (acpi_apic_t::local_apic_address_override_entry *)ent;
                    break;
                }
                default:
                    break;
                }
                start_addr += ent->length;
            }
        }
        if (!strncmp(entry->header.signature, "FACP", 4))
        {
            printk("find FACP\n");
        }
    }
}

void RSDT::Init()
{
    auto rsdp = RSDP::GetInstance();

    switch (rsdp->ACPIVersion())
    {
    case 1:
        do_init<acpi_rsdt_t>(rsdp);
        break;
    case 2:
        do_init<acpi_xsdt_t>(rsdp);
        break;
    default:
        break;
    }
}
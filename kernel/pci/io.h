#pragma once
#include <std/stdint.h>
#include <std/port_ops.h>
#include <std/printk.h>

constexpr uint8_t CONFIG_ADDRESS = 0xcf8;
constexpr uint8_t CONFIG_DATA = 0xcfc;

struct CONFIG_ADDRESS_STRUCT
{
    uint32_t register_offset : 8;
    uint32_t function_number : 3;
    uint32_t device_number : 5;
    uint32_t bus_number : 8;
    uint32_t reserved : 7;
    uint32_t enable_bit : 1;
};

uint32_t pci_read(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    offset &= 0xfc;

    CONFIG_ADDRESS_STRUCT config = {
        .register_offset = offset,
        .function_number = func,
        .device_number = slot,
        .bus_number = bus,
        .reserved = 0,
        .enable_bit = 1};

    uint64_t address;
    uint64_t lbus = (uint64_t)bus;
    uint64_t lslot = (uint64_t)slot;
    uint64_t lfunc = (uint64_t)func;
    uint16_t tmp = 0;
    address = (uint64_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    auto val = *(uint32_t *)&config;
    outl(0xCF8, address);
    // tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return inl(0xCFC);
}

uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0);
    return r0;
}

uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 2);
    return r0 >> 16;
}

uint16_t getStatus(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0x4);
    return r0 >> 16;
}

uint16_t getCommand(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0x4);
    return r0;
}

uint8_t getClassId(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0x8);
    return r0 >> 24;
}

uint8_t getSubClassId(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0x8);
    return (r0 & 0x00ffffff) >> 16;
}

uint8_t getProgIF(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0x8);
    return (r0 & 0x0000ffff) >> 8;
}

uint8_t getRevisionID(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0x8);
    return r0;
}

uint8_t getBIST(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0xc);
    return r0 >> 24;
}

uint8_t getHeaderType(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0xc);
    return (r0 & 0x00ffffff) >> 16;
}

uint8_t getLatencyTimer(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0xc);
    return (r0 & 0x0000ffff) >> 8;
}

uint8_t getCacheLineSize(uint16_t bus, uint16_t device, uint16_t function)
{
    uint32_t r0 = pci_read(bus, device, function, 0xc);
    return r0;
}

void pci_probe()
{
    for (uint32_t bus = 0; bus < 256; bus++)
    {
        for (uint32_t slot = 0; slot < 32; slot++)
        {
            for (uint32_t function = 0; function < 8; function++)
            {
                uint16_t vendor = getVendorID(bus, slot, function);
                if (vendor == 0xffff)
                    continue;
                uint16_t device = getDeviceID(bus, slot, function);
                uint16_t status = getStatus(bus, slot, function);
                uint16_t command = getCommand(bus, slot, function);
                uint16_t class_id = getClassId(bus, slot, function);
                auto subclass_id = getSubClassId(bus, slot, function);
                auto prog_if = getProgIF(bus, slot, function);
                auto revision_id = getRevisionID(bus, slot, function);
                auto bist = getBIST(bus, slot, function);
                auto header_type = getHeaderType(bus, slot, function);
                auto latency_timer = getLatencyTimer(bus, slot, function);
                auto cl_size = getCacheLineSize(bus, slot, function);
                printk("vendor: %x device: %x\n", vendor, device);
                printk("    status: %d command: %d\n", status, command);
                printk("    class_id: %d subclass_id: %d prog_if: %d revision_id: %d\n", class_id, subclass_id, prog_if, revision_id);
                printk("    bist: %d header_type: %d latency_timer: %d cl_size: %d\n", bist, header_type, latency_timer, cl_size);
            }
        }
    }
}
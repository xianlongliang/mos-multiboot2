#pragma once

#include <std/stdint.h>
#include <std/singleton.h>

class RSDP : public Singleton<RSDP>
{
public:
  void Init(uint8_t rsdp_version, uint8_t*rsdp_address);

  // RSDTAddress round down to 4K page boundary
  uint8_t*RSDTAddressBase();

  // return the actual address of rsdt
  uint8_t*RSDTAddress();

  uint8_t ACPIVersion() { return this->acpi_version; }

private:
  uint8_t acpi_version;
  // if acpi_version == 1, the addresses below are rsdt
  // otherwise they are xsdt
  uint8_t*rsdt_vaddr_base;
  uint8_t*rsdt_vaddr;
};

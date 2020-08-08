#pragma once

#include <std/stdint.h>
#include <std/singleton.h>

class RSDP : public Singleton<RSDP>
{
public:
  void Init(uint8_t rsdp_version, void *rsdp_address);

  // RSDTAddress round down to 4K page boundary
  void *RSDTAddressBase();

  // return the actual address of rsdt
  void *RSDTAddress();

  uint8_t ACPIVersion() { return this->acpi_version; }

private:
  uint8_t acpi_version;
  // if acpi_version == 1, the addresses below are rsdt
  // otherwise they are xsdt
  void *rsdt_vaddr_base;
  void *rsdt_vaddr;
};

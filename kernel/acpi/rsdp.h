#pragma once

#include <std/stdint.h>
#include <std/singleton.h>

class RSDP : public Singleton<RSDP> {
public:
  
  void Init(uint8_t rsdp_version, void *rsdp_address);

  // RSDTAddress round down to 4K page boundary
  void* RSDTAddressBase();

  // return the actual address of rsdt
  void* RSDTAddress();

private:
  void* rsdt_vaddr_base;
  void* rsdt_vaddr;
};

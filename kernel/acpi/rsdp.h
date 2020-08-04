#pragma once

#include <std/stdint.h>
#include <std/singleton.h>

class RSDP : public Singleton<RSDP> {
public:
  RSDP() {}
  void Init(uint8_t rsdp_version, void *rsdp_address);
  void* RSDTAddressBase();
  void* RSDTAddress();
private:
  void* rsdt_vaddr_base;
  void* rsdt_vaddr;
};

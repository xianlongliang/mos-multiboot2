#pragma once

#include <std/singleton.h>
#include "cpu.h"
#include <interrupt/apic.h>
#include <interrupt/pit.h>
#include <memory/physical_page.h>

class SMP : public Singleton<SMP>
{
public:
    void Init();

private:
};

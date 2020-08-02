#pragma once

#include <std/singleton.h>

class APIC : public Singleton<APIC>
{

public:
    APIC();
    // init local_apic and io_apic
    void Init();
};
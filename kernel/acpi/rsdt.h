#pragma once
#include <std/stdint.h>
#include <std/singleton.h>

class RSDT : public Singleton<RSDT>
{
public:
    void Init();
};
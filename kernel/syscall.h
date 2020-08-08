#pragma once

#include <std/singleton.h>

class Syscall : public Singleton<Syscall>
{
public:
    void Init();
};
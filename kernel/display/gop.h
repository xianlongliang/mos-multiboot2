#pragma once

#include <std/stdint.h>

class GOP
{

    enum Color
    {
        BLACK = 0x0,
        WHITE = 0xFFFFFFFF,
        BLUE = 0xFFFF,
    };

public:
    static void Init(void *address, uint16_t width, uint16_t height, uint16_t pitch);

    static void Clear();

    static void PutChar(char c, Color back = BLACK, Color fore = WHITE);

    static void PutString(char* str, Color back = BLACK, Color fore = WHITE);

private:
    static void scroll();
};
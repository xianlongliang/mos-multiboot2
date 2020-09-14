#pragma once

#include <std/stdint.h>

class RTC {

public:

    static uint8_t Second();

    static uint8_t Minute();

    static uint8_t Hour();

    static uint8_t Day();

    static uint8_t WeekDay();

    static uint8_t Month();

    static uint8_t Year();

    static void PrintTime();
};
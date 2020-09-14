#include "rtc.h"
#include <std/port_ops.h>
#include <std/printk.h>
#include <std/interrupt.h>

uint8_t RTC::Second()
{
    outb(0x70, 0x00);
    auto second = inb(0x71);
    second = (second & 0x0F) + ((second / 16) * 10);
    return second;
}

uint8_t RTC::Minute()
{
    outb(0x70, 0x02);
    auto minute = inb(0x71);
    minute = (minute & 0x0F) + ((minute / 16) * 10);
    return minute;
}

uint8_t RTC::Hour()
{
    outb(0x70, 0x04);
    auto hour = inb(0x71);
    hour = (hour & 0x0F) + ((hour / 16) * 10);
    return hour;
}

uint8_t RTC::Day()
{

    outb(0x70, 0x07);
    auto day = inb(0x71);
    day = (day & 0x0F) + ((day / 16) * 10);
    return day;
}

uint8_t RTC::WeekDay()
{
    outb(0x70, 0x06);
    return inb(0x71);
}

uint8_t RTC::Month()
{
    outb(0x70, 0x08);
    auto month = inb(0x71);
    month = (month & 0x0F) + ((month / 16) * 10);
    return month;
}

uint8_t RTC::Year()
{
    outb(0x70, 0x09);
    auto year = inb(0x71);
    year = (year & 0x0F) + ((year / 16) * 10);
    return year;
}

void RTC::PrintTime()
{
    printk("20%d-%d-%d %d:%d:%d\n",
           Year(), Month(), Day(),
           Hour(), Minute(), Second());
}
#pragma once
#include <std/stdint.h>
#include <stdarg.h>

namespace Kernel::VGA
{
    enum class Color : uint8_t
    {
        rc_black = 0,
        rc_blue = 1,
        rc_green = 2,
        rc_cyan = 3,
        rc_red = 4,
        rc_magenta = 5,
        rc_brown = 6,
        rc_light_grey = 7,
        rc_dark_grey = 8,
        rc_light_blue = 9,
        rc_light_green = 10,
        rc_light_cyan = 11,
        rc_light_red = 12,
        rc_light_magenta = 13,
        rc_light_brown = 14, // yellow
        rc_white = 15
    };

    // 清屏操作
    void console_clear();

    // 屏幕输出一个字符  带颜色
    void console_putc_color(char c, Color back, Color fore);

    // 屏幕打印一个以 \0 结尾的字符串  默认黑底白字
    void console_write(char *cstr);

    // 屏幕打印一个以 \0 结尾的字符串  带颜色
    void console_putc_color(char c, Color back = Color::rc_black, Color fore = Color::rc_white);

    // 屏幕输出一个十六进制的整型数
    void console_write_hex(uint32_t n, Color back, Color fore);

    // 屏幕输出一个十进制的整型数
    void console_write_dec(uint32_t n, Color back, Color fore);
} // namespace Kernel::VGA

extern "C"
{
    void printk(const char *format, ...);
    void clear();
    void printk_with_spinlock(const char *format, ...);
    void printk_with_spinlock_cli(const char *format, ...);

    void printk_while(const char *format, ...);

    void printk_color(Kernel::VGA::Color back, Kernel::VGA::Color fore, const char *format, ...);
}
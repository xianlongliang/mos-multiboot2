#include "printk.h"
#include "port_ops.h"
#include "string.h"
#include "spinlock.h"

static Spinlock printk_spinlock;

namespace Kernel::VGA
{
        // VGA 的显示缓冲的起点是 0xB8000
        static uint16_t *video_memory = (uint16_t *)(0xFFFFFFFF00000000 + 0xb8000);

        // 屏幕"光标"的坐标
        static uint8_t cursor_x = 0;
        static uint8_t cursor_y = 0;

        static void move_cursor()
        {
                // 屏幕是 80 字节宽
                uint16_t cursorLocation = cursor_y * 80 + cursor_x;

                // 在这里用到的两个内部寄存器的编号为14与15，分别表示光标位置
                // 的高8位与低8位。

                outb(0x3D4, 14);                  // 告诉 VGA 我们要设置光标的高字节
                outb(0x3D5, cursorLocation >> 8); // 发送高 8 位
                outb(0x3D4, 15);                  // 告诉 VGA 我们要设置光标的低字节
                outb(0x3D5, cursorLocation);      // 发送低 8 位
        }

        void console_clear()
        {
                uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
                uint16_t blank = 0x20 | (attribute_byte << 8);

                int i;
                for (i = 0; i < 80 * 25; i++)
                {
                        video_memory[i] = blank;
                }

                cursor_x = 0;
                cursor_y = 0;
                move_cursor();
        }

        static void scroll()
        {
                // attribute_byte 被构造出一个黑底白字的描述格式
                uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
                uint16_t blank = 0x20 | (attribute_byte << 8); // space 是 0x20

                // cursor_y 到 25 的时候，就该换行了
                if (cursor_y >= 25)
                {
                        // 将所有行的显示数据复制到上一行，第一行永远消失了...
                        int i;

                        for (i = 0 * 80; i < 24 * 80; i++)
                        {
                                video_memory[i] = video_memory[i + 80];
                        }

                        // 最后的一行数据现在填充空格，不显示任何字符
                        for (i = 24 * 80; i < 25 * 80; i++)
                        {
                                video_memory[i] = blank;
                        }

                        // 向上移动了一行，所以 cursor_y 现在是 24
                        cursor_y = 24;
                }
        }

        void console_putc_color(char c, Color back, Color fore)
        {
                uint8_t back_color = (uint8_t)back;
                uint8_t fore_color = (uint8_t)fore;

                uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
                uint16_t attribute = attribute_byte << 8;

                // 0x08 是退格键的 ASCII 码
                // 0x09 是tab 键的 ASCII 码
                if (c == 0x08 && cursor_x)
                {
                        cursor_x--;
                        video_memory[cursor_y * 80 + cursor_x] = ' ' | attribute;
                }
                else if (c == 0x09)
                {
                        cursor_x = (cursor_x + 8) & ~(8 - 1);
                }
                else if (c == '\r')
                {
                        cursor_x = 0;
                        cursor_y++;
                }
                else if (c == '\n')
                {
                        cursor_x = 0;
                        cursor_y++;
                }
                else if (c >= ' ')
                {
                        video_memory[cursor_y * 80 + cursor_x] = c | attribute;
                        cursor_x++;
                }

                // 每 80 个字符一行，满80就必须换行了
                if (cursor_x >= 80)
                {
                        cursor_x = 0;
                        cursor_y++;
                }

                // 如果需要的话滚动屏幕显示
                scroll();

                // 移动硬件的输入光标
                move_cursor();
        }

        void console_write(char *cstr)
        {
                while (*cstr)
                {
                        console_putc_color(*cstr++, Color::rc_black, Color::rc_white);
                }
        }

        void console_write_color(char *cstr, Color back, Color fore)
        {
                while (*cstr)
                {
                        console_putc_color(*cstr++, back, fore);
                }
        }
} // namespace Kernel::VGA

void putchar(char ch)
{
        Kernel::VGA::console_putc_color(ch);
}

void putvalue(char *val)
{
        while (*val != '\0')
        {
                Kernel::VGA::console_putc_color(*val);
                ++val;
        }
}

char *int2hexstr(uint64_t val, int &end_index)
{
        static const auto hex_table = "0123456789abcdef";
        static char buff[32];
        int i = 0;
        while (val != 0)
        {
                auto hex = val % 16;
                val = val / 16;
                buff[i++] = hex_table[hex];
        }
        end_index = i > 0 ? i - 1 : 0;
        if (i == 0)
                buff[0] = '0';
        return buff;
}

char *int2str(uint64_t val, int &end_index)
{
        static char buff[32] = {'0'};
        int i = 0;
        while (val != 0)
        {
                auto hex = val % 10;
                val = val / 10;
                buff[i++] = hex + '0';
        }
        end_index = i > 0 ? i - 1 : 0;
        if (i == 0)
                buff[0] = '0';
        return buff;
}

void putvalue(uint64_t val)
{
        int end_idx;
        auto buff = int2str(val, end_idx);
        for (int i = end_idx; i >= 0; --i)
        {
                Kernel::VGA::console_putc_color(buff[i]);
        }
}

void putvalue(int64_t val)
{
        if (val < 0)
        {
                Kernel::VGA::console_putc_color('-');
                putvalue(uint64_t(-val));
                return;
        }

        putvalue(uint64_t(val));
}

void putvalue(void *val)
{
        int end_idx;
        auto buff = int2hexstr(uint64_t(val), end_idx);

        Kernel::VGA::console_putc_color('0');
        Kernel::VGA::console_putc_color('x');
        auto padding = 16 - end_idx;
        while (padding--)
        {
                Kernel::VGA::console_putc_color('0');
        }
        for (int i = end_idx; i >= 0; --i)
        {
                Kernel::VGA::console_putc_color(buff[i]);
        }
}
void clear()
{
        Kernel::VGA::console_clear();
}

void printk(const char *format, ...)
{
        printk_spinlock.lock();

        asm volatile("pushf");
        asm volatile("cli");
        static const auto peek = [](char *current, uint64_t count = 1) {
                return current[count];
        };

        va_list args;
        va_start(args, format);
        auto pformat = const_cast<char *>(format);
        char ch = *format;
        while (ch)
        {
                if (ch == '%')
                {
                        switch (peek(pformat))
                        {
                        // int64_t
                        case 'd':
                        {
                                ++pformat;
                                putvalue((int64_t)va_arg(args, int64_t));
                                break;
                        }
                        case 's':
                        {
                                ++pformat;
                                putvalue(va_arg(args, char *));
                                break;
                        }
                        case 'u':
                        {
                                ++pformat;
                                putvalue(va_arg(args, uint64_t));
                                break;
                        }
                        // case 'f':
                        // {
                        //         ++format;
                        //         putvalue(va_arg(args, double));
                        //         break;
                        // }
                        case 'x':
                        case 'p':
                        {
                                ++pformat;
                                putvalue(va_arg(args, void *));
                                break;
                        }
                        default:
                        {
                                // output '%'
                                putchar(ch);
                        }
                        }
                }
                else
                {
                        // output ch
                        putchar(ch);
                }
                ch = *(++pformat);
        }
        va_end(args);

        asm volatile("popf");
        printk_spinlock.unlock();

}

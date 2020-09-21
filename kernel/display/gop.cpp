#include "gop.h"
#include <std/kstring.h>

#undef __cplusplus
#define SSFN_CONSOLEBITMAP_TRUECOLOR /* use the special renderer for hicolor packed pixels */
#include <ssfn.h>
extern "C" unsigned char _binary_u_vga16_sfn_start;
#define __cplusplus

void GOP::Init(void *address, uint16_t width, uint16_t height, uint16_t pitch)
{
    /* set up context by global variables */
    ssfn_src = (ssfn_font_t *)&_binary_u_vga16_sfn_start; /* the bitmap font to use */

    ssfn_dst.ptr = (uint8_t *)address; /* address of the linear frame buffer */
    ssfn_dst.w = width;                /* width */
    ssfn_dst.h = height;               /* height */
    ssfn_dst.p = pitch;                /* bytes per line */
    ssfn_dst.x = 0;                    /* pen position */
    ssfn_dst.y = 0;                    /* pen position */
    ssfn_dst.bg = 0x0;
    ssfn_dst.fg = 0xFFFFFFFF; /* foreground color */

    GOP::Clear();
}

inline void do_backspace() {
    // font height 16 pixels
    auto y_start = ssfn_dst.y - 1;
    for (int i = 0; i < 16; i++) {
        auto start = y_start * ssfn_dst.w * sizeof(uint32_t) + (ssfn_dst.x - 1) * sizeof(uint32_t);
        uint32_t* ptr = (uint32_t*)(ssfn_dst.ptr + start);
        for (int j = 0; j < 8; ++i) {
            ptr[0] = 0x0;
        }
    }
}

void GOP::PutChar(char c, Color back, Color fore)
{

    ssfn_dst.bg = back;
    ssfn_dst.fg = fore;

    // 0x08 是退格键的 ASCII 码
    // 0x09 是tab 键的 ASCII 码
    auto &cursor_x = ssfn_dst.x;
    auto &cursor_y = ssfn_dst.y;
    // Backspace
    if (c == 0x08 && cursor_x)
    {
        cursor_x -= 8;
        auto fg_bak = ssfn_dst.fg;
        ssfn_dst.fg = ssfn_dst.bg;
        ssfn_putc(0x08);
        ssfn_dst.fg = fg_bak;
        cursor_x -= 8;
        return;
    }
    // Tab
    else if (c == 0x09)
    {
        cursor_x = (cursor_x + 8) & ~(8 - 1);
    }
    else if (c == '\r')
    {
        cursor_x = 0;
        cursor_y += 16;
    }
    else if (c == '\n')
    {
        cursor_x = 0;
        cursor_y += 16;
    }

    if (cursor_x >= ssfn_dst.w)
    {
        cursor_x = 0;
        cursor_y += 16;
    }
    scroll();
    ssfn_putc(c);
}

void GOP::PutString(char *str, Color back, Color fore)
{
    while (*str)
    {
        GOP::PutChar(*str++, back, fore);
    }
}

void GOP::Clear()
{
    hzero(ssfn_dst.ptr, ssfn_dst.w * ssfn_dst.h / 4);
    ssfn_dst.x = 0; /* pen position */
    ssfn_dst.y = 0; /* pen position */
}

void GOP::scroll()
{
    auto &cursor_y = ssfn_dst.y;
    auto video_memory = (uint32_t *)ssfn_dst.ptr;
    auto x_count = ssfn_dst.w;
    auto y_count = ssfn_dst.h;

    // cursor_y 到 25 的时候，就该换行了
    if (cursor_y + 16 >= ssfn_dst.h)
    {
        // 将所有行的显示数据复制到上一行，第一行永远消失了...
        int i;

        auto last_pixel = y_count * x_count - 16 * x_count;
        for (i = 0; i <= last_pixel; i++)
        {
            video_memory[i] = video_memory[i + 16 * x_count];
        }

        auto end_line = y_count * x_count - 1;
        // 最后的一行数据现在填充空格，不显示任何字符
        for (i = last_pixel + 1; i < end_line; i++)
        {
            video_memory[i] = 0x0;
        }

        // 向上移动了一行，所以 cursor_y 现在是 24
        cursor_y -= 16;
    }
}

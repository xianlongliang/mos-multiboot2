#pragma once
#include <std/stdint.h>

// write value to port (1 byte)
inline void outb(uint16_t port, uint8_t value)
{
    asm volatile("outb %1, %0"
                 :
                 : "dN"(port), "a"(value));
}

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;

    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "dN"(port));

    return ret;
}

inline uint16_t inw(uint16_t port)
{
    uint16_t ret;

    asm volatile("inw %1, %0"
                 : "=a"(ret)
                 : "dN"(port));

    return ret;
}

inline void insw(uint16_t port, void *addr, uint32_t word_cnt)
{
    asm volatile("cld; rep insw"
                 : "+D"(addr), "+c"(word_cnt)
                 : "d"(port)
                 : "memory");
}

inline void outsw(uint16_t port, const void *addr, uint32_t word_cnt)
{
    asm volatile("cld; rep outsw"
                 : "+S"(addr), "+c"(word_cnt)
                 : "d"(port));
}
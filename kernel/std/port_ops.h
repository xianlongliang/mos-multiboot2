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

inline void outw(uint16_t port, uint16_t value)
{
    asm volatile("outw %1, %0"
                 :
                 : "dN"(port), "a"(value));
}

inline uint32_t inl(uint16_t port)
{
    uint32_t ret;

    asm volatile("inl %1, %0"
                 : "=a"(ret)
                 : "dN"(port));

    return ret;
}

inline void outl(uint16_t port, uint32_t value)
{
    asm volatile("outl %1, %0"
                 :
                 : "dN"(port), "a"(value));
}

inline void insw(uint16_t port, uint8_t *addr, uint32_t word_cnt)
{
    asm volatile("cld; rep insw"
                 : "+D"(addr), "+c"(word_cnt)
                 : "d"(port)
                 : "memory");
}

inline void outsw(uint16_t port, const uint8_t *addr, uint32_t word_cnt)
{
    asm volatile("cld; rep outsw"
                 : "+S"(addr), "+c"(word_cnt)
                 : "d"(port));
}
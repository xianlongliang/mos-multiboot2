#include "string.h"

inline void memcpy(void *dst, const void *src, uint64_t len)
{
    uint8_t *cdst = (uint8_t *)dst;
    uint8_t *csrc = (uint8_t *)src;
    for (; len != 0; len--)
    {
        *cdst++ = *csrc++;
    }
}

inline void memmove(void *dst, const void *src, uint64_t len)
{
    uint8_t *cdst = (uint8_t *)dst;
    uint8_t *csrc = (uint8_t *)src;
    for (; len != 0; len--)
    {
        *cdst++ = *csrc++;
    }
}

inline void memset(void *dst, uint8_t val, uint64_t len)
{
    uint8_t *cdst = (uint8_t *)dst;
    for (; len != 0; len--)
    {
        *cdst++ = val;
    }
}

inline void bzero(void *dest, uint64_t len)
{
    memset(dest, 0, len);
}

inline int strcmp(const char *s1, const char *s2)
{
    int8_t res = 0;
    while (*s1 && !(res = *(uint8_t *)s2 - *(uint8_t *)s1))
    {
        ++s1;
        ++s2;
    }
    if (res < 0)
        res = -1;
    else if (res > 0)
        res = 1;
    return res;
}

inline char *strcpy(char *dst, const char *src)
{
    char *tmp = dst;
    while (*src)
    {
        *dst = *src;
        ++dst;
        ++src;
    }
    *dst = '\0';
    return tmp;
}

inline char *strncpy(char *dest, const char *src, uint32_t len)
{
    char *dst = dest;
    while (len > 0)
    {
        while (*src)
        {
            *dest++ = *src++;
        }
        len--;
    }
    *dest = '\0';
    return dst;
}

inline char *strcat(char *dest, const char *src)
{
    char *cp = dest;

    while (*cp)
    {
        cp++;
    }

    while ((*cp++ = *src++))
        ;

    return dest;
}

inline int strlen(const char *src)
{
    const char *eos = src;

    while (*eos++)
        ;

    return (eos - src - 1);
}
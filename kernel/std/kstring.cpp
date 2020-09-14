#include "kstring.h"
#include <std/stdint.h>

void memcpy(void *dst, const void *src, uint64_t len)
{
    uint8_t *cdst = (uint8_t *)dst;
    uint8_t *csrc = (uint8_t *)src;
    for (; len != 0; len--)
    {
        *cdst++ = *csrc++;
    }
}

void memmove(void *dst, const void *src, uint64_t len)
{
    uint8_t *cdst = (uint8_t *)dst;
    uint8_t *csrc = (uint8_t *)src;
    if (cdst > csrc && cdst < csrc + len)
    {
        uint8_t *right_d_start = cdst + len - 1;
        uint8_t *right_s_start = csrc + len - 1;
        for (int i = 0; i < len; ++i)
        {
            *(right_d_start - i) = *(right_s_start - i);
        }
        return;
    }

    if (csrc > cdst && csrc < cdst + len)
    {
        uint8_t *left_d_start = cdst;
        uint8_t *left_s_start = csrc;
        for (int i = 0; i < len; ++i)
        {
            *(left_d_start + i) = *(left_s_start + i);
        }
        return;
    }

    memcpy(dst, src, len);
}

void memset(void *dst, uint8_t val, uint64_t len)
{
    uint8_t *cdst = (uint8_t *)dst;
    for (; len != 0; len--)
    {
        *cdst++ = val;
    }
}

void bzero(void *dest, uint64_t len)
{
    memset(dest, 0, len);
}

int strcmp(const char *s1, const char *s2)
{
    int8_t res = 0;
    while (!(res = *(uint8_t *)s2 - *(uint8_t *)s1) && *s1 != '\0')
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

int strncmp(const char *s1, const char *s2, int n)
{
    int8_t res = 0;
    while (n > 0 && *s1 && !(res = *(uint8_t *)s2 - *(uint8_t *)s1))
    {
        ++s1;
        ++s2;
        --n;
    }
    if (res < 0)
        res = -1;
    else if (res > 0)
        res = 1;
    return res;
}

char *strcpy(char *dst, const char *src)
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

char *strncpy(char *dest, const char *src, uint32_t len)
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

char *strcat(char *dest, const char *src)
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

int strlen(const char *src)
{
    const char *eos = src;

    while (*eos++)
        ;

    return (eos - src - 1);
}
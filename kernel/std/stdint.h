#pragma once

#define NO_ALIGNMENT __attribute__((packed))

typedef char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;
typedef long unsigned int size_t;
typedef long signed int ssize_t;
typedef __int128_t int128_t;
typedef __uint128_t uint128_t;

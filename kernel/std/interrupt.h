#pragma once

inline void cli() {
    asm volatile("cli");
}

inline void sti() {
    asm volatile("sti");
}

#define CLI_GUARD(exp) \
            cli(); \
            exp;   \
            sti() \
            

inline void hlt() {
    asm volatile("hlt");
}
#pragma once

inline void cli() {
    asm volatile("cli");
}

inline void sti() {
    asm volatile("sti");
}
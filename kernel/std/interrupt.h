#pragma once

inline void cli() {
    asm volatile("cli");
}

inline void sti() {
    asm volatile("sti");
}

inline void hlt() {
    asm volatile("hlt");
}
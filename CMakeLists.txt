cmake_minimum_required(VERSION 3.16)
project(kernel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "-fno-pie -no-pie -g -std=gnu++17 -fno-builtin -fno-exceptions -fno-rtti -fno-threadsafe-statics -nostdlib -ffreestanding -g -Wall -Wextra -MMD -mno-red-zone -mcmodel=kernel -Wl,--build-id=none -T ${CMAKE_SOURCE_DIR}/kernel/kernel.ld")
set(CMAKE_C_FLAGS "-fno-pie -no-pie -g -mcmodel=kernel")

set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
ENABLE_LANGUAGE(ASM_NASM)

include_directories(${CMAKE_SOURCE_DIR}/kernel)
add_executable(kernel 
        kernel/boot.asm
        kernel/kernel.h 
        kernel/kernel.cpp 
        kernel/setup.h 
        kernel/setup.cpp 
        kernel/regs.h
        kernel/tss.h
        kernel/tss.cpp
        kernel/gdt.h
        kernel/gdt.cpp
        kernel/task.h
        kernel/task.cpp
        kernel/task.asm
        kernel/std/printk.h 
        kernel/std/printk.cpp 
        kernel/std/kstring.h 
        kernel/std/kstring.cpp
        kernel/std/debug.h
        kernel/std/debug.cpp
        kernel/memory/zone.h
        kernel/memory/zone.cpp
        kernel/memory/virtual_page.h
        kernel/memory/physical_page.h
        kernel/memory/physical.h
        kernel/memory/physical.cpp
        kernel/memory/kernel_page.h
        kernel/memory/kernel_page.cpp
        kernel/interrupt/idt.h
        kernel/interrupt/idt.cpp
        kernel/interrupt/isr.asm
        kernel/interrupt/irq.asm
        kernel/interrupt/page_fault.cpp
        kernel/interrupt/timer.h
        kernel/interrupt/timer.cpp
        )

add_custom_command(TARGET kernel
        POST_BUILD
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_SOURCE_DIR}/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/
        COMMAND cp ${CMAKE_BINARY_DIR}/kernel ${CMAKE_BINARY_DIR}/iso/boot/
        COMMAND grub-mkrescue -o kernel.iso ${CMAKE_BINARY_DIR}/iso
        )
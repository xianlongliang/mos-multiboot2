cmake_minimum_required(VERSION 3.16)
project(kernel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "-fno-pie -no-pie -g -std=gnu++17 -fno-builtin -fno-exceptions -fno-rtti -fno-threadsafe-statics -nostdlib -ffreestanding -g -Wall -Wextra -MMD -mno-red-zone -mcmodel=kernel -Wl,--build-id=none -T ${CMAKE_SOURCE_DIR}/kernel/kernel.ld")
set(CMAKE_C_FLAGS "-fno-pie -no-pie -g -mcmodel=kernel")

include_directories(${CMAKE_SOURCE_DIR}/kernel)
set_property(SOURCE kernel/boot.S PROPERTY LANGUAGE C)
add_executable(kernel 
        kernel/boot.S
        kernel/kernel.h 
        kernel/kernel.cpp 
        kernel/std/printk.h 
        kernel/std/printk.cpp 
        kernel/std/string.h 
        kernel/std/string.cpp
        kernel/std/debug.h
        kernel/std/debug.cpp
        kernel/memory/zone.h
        kernel/memory/zone.cpp
        kernel/memory/physical.h
        kernel/memory/physical.cpp
        )

add_custom_command(TARGET kernel
        POST_BUILD
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_SOURCE_DIR}/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/
        COMMAND cp ${CMAKE_BINARY_DIR}/kernel ${CMAKE_BINARY_DIR}/iso/boot/
        COMMAND grub-mkrescue -o kernel.iso ${CMAKE_BINARY_DIR}/iso
        )
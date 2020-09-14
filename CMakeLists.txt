cmake_minimum_required(VERSION 3.16)
project(kernel)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_FLAG "-g -static -march=core2 -fno-exceptions -fno-builtin -fno-rtti -fno-threadsafe-statics -nostdlib -ffreestanding -g -Wall -Wextra -MMD -mno-red-zone -mcmodel=large")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${BUILD_FLAG}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld ${CMAKE_SOURCE_DIR}/kernel/kernel.ld")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS  "-fconcepts -no-pie ${BUILD_FLAG} -T ${CMAKE_SOURCE_DIR}/kernel/kernel.ld")
endif()
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS nasm asm)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
ENABLE_LANGUAGE(ASM_NASM)

include_directories(${CMAKE_SOURCE_DIR}/kernel)
add_executable(kernel 

        kernel/boot.asm
        kernel/kernel.h 
        kernel/kernel.cpp 
        kernel/syscall.h 
        kernel/syscall.cpp 
        kernel/syscall.asm 
        kernel/setup.h 
        kernel/setup.cpp 
        kernel/tss.h
        kernel/tss.cpp
        kernel/gdt.h
        kernel/gdt.cpp

        kernel/smp/cpu.h
        kernel/smp/cpu.cpp
        kernel/smp/smp.h
        kernel/smp/smp.cpp

        kernel/acpi/rsdp.h
        kernel/acpi/rsdp.cpp
        kernel/acpi/rsdt.h
        kernel/acpi/rsdt.cpp
        
        kernel/thread/regs.h
        kernel/thread/condition_variable.h
        kernel/thread/condition_variable.cpp
        kernel/thread/io_queue.h
        kernel/thread/mutex.h
        kernel/thread/mutex.cpp
        kernel/thread/semaphore.h
        kernel/thread/semaphore.cpp
        kernel/thread/scheduler.h
        kernel/thread/scheduler.cpp
        kernel/thread/task.h
        kernel/thread/task.cpp
        kernel/thread/task.asm
        kernel/thread/userland.h
        kernel/thread/userland.cpp
        kernel/std/singleton.h 
        kernel/std/initializer_list.h
        kernel/std/string.h 
        kernel/std/allocator.h 
        kernel/std/unordered_set.h 
        kernel/std/new.h 
        kernel/std/new.cpp 
        kernel/std/map.h 
        kernel/std/atomic.h 
        kernel/std/avl_tree.h 
        kernel/std/msr.h 
        kernel/std/vector.h 
        kernel/std/list.h 
        kernel/std/math.h 
        kernel/std/math.cpp
        kernel/std/printk.h 
        kernel/std/printk.cpp 
        kernel/std/kstring.h 
        kernel/std/kstring.cpp
        kernel/std/debug.h
        kernel/std/debug.cpp
        kernel/std/interrupt.h
        kernel/std/bitmap.h
        kernel/std/bitmap.cpp
        kernel/std/move.h
        kernel/std/unique_ptr.h
        kernel/std/shared_ptr.h
        kernel/std/c++/pure_virtual_function.cpp
        kernel/std/c++/global_object.cpp
        
        kernel/memory/lrmalloc/lrmalloc.h
        kernel/memory/lrmalloc/lrmalloc.cpp
        kernel/memory/lrmalloc/heap.h
        kernel/memory/lrmalloc/heap.cpp

        kernel/memory/lrmalloc/descriptor.h
        kernel/memory/lrmalloc/descriptor.cpp
        kernel/memory/lrmalloc/lrmalloc.h
        kernel/memory/lrmalloc/cache_bin.h
        kernel/memory/lrmalloc/cache_bin.cpp
        kernel/memory/lrmalloc/size_class.h
        kernel/memory/lrmalloc/size_class.cpp
        kernel/memory/zone.h
        kernel/memory/zone.cpp
        kernel/memory/virtual_page.h
        kernel/memory/physical_page.h
        kernel/memory/physical.h
        kernel/memory/physical.cpp
        kernel/memory/kernel_page.h
        kernel/memory/kernel_page.cpp
        kernel/memory/slab.h
        kernel/memory/mapping.h
        kernel/memory/mapping.cpp
        kernel/memory/slab.cpp
        kernel/memory/heap.h
        kernel/memory/heap.cpp
        kernel/memory/kmalloc.h
        kernel/memory/kmalloc.cpp
        kernel/interrupt/pit.h
        kernel/interrupt/pit.cpp
        kernel/interrupt/io_apic.h
        kernel/interrupt/io_apic.cpp
        kernel/interrupt/apic.h
        kernel/interrupt/apic.cpp
        kernel/interrupt/idt.h
        kernel/interrupt/idt.cpp
        kernel/interrupt/isr.asm
        kernel/interrupt/irq.asm
        kernel/interrupt/page_fault.cpp
        kernel/interrupt/keyboard.h
        kernel/interrupt/keyboard.cpp
        kernel/syscall/read.cpp
        kernel/syscall/write.cpp
        kernel/display/gop.h
        kernel/display/gop.cpp

        kernel/pci/io.h
        kernel/rtc/rtc.h
        kernel/rtc/rtc.cpp

        u_vga16.o
        )

add_custom_command(TARGET kernel
        POST_BUILD
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_SOURCE_DIR}/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/
        COMMAND cp ${CMAKE_BINARY_DIR}/kernel ${CMAKE_BINARY_DIR}/iso/boot/
        COMMAND cp -r ${CMAKE_SOURCE_DIR}/EFI ${CMAKE_BINARY_DIR}/iso/
        COMMAND grub-mkrescue -o kernel.iso ${CMAKE_BINARY_DIR}/iso
        )
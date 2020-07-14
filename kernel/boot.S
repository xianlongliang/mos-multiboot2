#include "arch/x86_64/gdt.h"
#include "arch/x86_64/mmu.h"
#include "kernel.h"
#include "sizes.h"
#include "multiboot2.h"
#include "arch/x86_64/msr.h"

.SET HEADER_LENGTH, header_end - header_start
.SET CHECKSUM, -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + HEADER_LENGTH)
.section .multiboot
header_start:
    .long MULTIBOOT2_HEADER_MAGIC
    .long MULTIBOOT_ARCHITECTURE_I386
    .long HEADER_LENGTH
    .long CHECKSUM

    // multiboot tags go here

    .short MULTIBOOT_HEADER_TAG_END
    .short 0    // flags, none set
    .long 8     // size, including itself (short + short + long)
header_end:

.code32

.section .bss
.comm pml4, 0x1000, 0x1000
.comm pdpe, 0x1000, 0x1000
.comm pde, 0x1000, 0x1000
.comm pte, 0x1000, 0x1000

.data
.align GDT_TABLE_ALIGNMENT
gdt_table:
        .8byte GDT_FIRST_ENTRY
        .8byte GDT_KERNEL_ENTRY

gdt_table_end:
        .skip (GDT_TABLE_SIZE - (gdt_table_end - gdt_table))

gdt_ptr:
         .short GDT_TABLE_SIZE - 1
         .long gdt_table


.section .text
.global _start
.type _start, @function
_start:

        movl $pdpe, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
        movl %eax, pml4 

        pushl %eax
        addl $0xff0, %eax
        movl %eax, pml4 + 0xff8
        pop %eax

        movl $pde, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
        movl %eax, pdpe
        movl %eax, pdpe + 0xff0

        movl $pte, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
        movl %eax, pde

        movl $0x200000, %ecx
        or $(MMU_PRESENT | MMU_WRITABLE), %ecx
        movl $0x0, %edi
        movl $0x0, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
page_directory_table_loop:
        movl %eax, pte(, %edi, 0x8)
        addl $0x1000, %eax
        addl $0x1, %edi
        cmp %eax, %ecx
        jne page_directory_table_loop  // if not equal redo loop

        movl $pml4, %eax
        movl %eax, %cr3

        movl $KERNEL_CR4, %eax
        movl %eax, %cr4

        movl $MSR_EFER, %ecx
        rdmsr
        or $MSR_EFER_LME, %eax
        wrmsr

        movl $KERNEL_CR0, %eax
        movl %eax, %cr0

        lgdt gdt_ptr
        
        ljmp $(KERNEL_GDT_ENTRY * GDT_ENTRY_SIZE), $_start64

        cli
        hlt

.code64

.global _start64
.type _start64, @function
_start64:
        
        // Setup segment selectors
        movw $0, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs
        movw %ax, %ss


        mov $0x7000, %rsp
        mov %rsp, %rbp

        movq %rbx, %rdi
        
        call Kernel_Main

        cli
        hlt
1:
        jmp 1b

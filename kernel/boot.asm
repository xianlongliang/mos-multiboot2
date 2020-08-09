PAGE_SIZE equ 0x1000
KERNEL_TEXT_BASE equ 0xFFFFFFFF00000000
%define PAGE_PRESENT    (1 << 0)
%define PAGE_WRITE      (1 << 1)
%define PAGE_USER       (1 << 2)
%define PAGE_1GB       (1 << 7)
%define CONTROL_REGISTER4_PHYSICAL_ADDRESS_EXTENSION (1 << 5)
%define KERNEL_CR4 (CONTROL_REGISTER4_PHYSICAL_ADDRESS_EXTENSION)
%define MSR_EFER 0xC0000080
%define MSR_EFER_LME (1 << 8)
%define MSR_EFER_SCE (1 << 0)

%define CONTROL_REGISTER0_PROTECTED_MODE_ENABLED (1 << 0)
%define CONTROL_REGISTER0_EXTENSION_TYPE (1 << 4)
%define CONTROL_REGISTER0_PAGE (1 << 31)
%define KERNEL_CR0 (CONTROL_REGISTER0_PAGE | CONTROL_REGISTER0_PROTECTED_MODE_ENABLED | CONTROL_REGISTER0_EXTENSION_TYPE)
%define CODE_SEG     0x0008
%define DATA_SEG     0x0010
 
section .multiboot_header
align 8
header_start:
    dd 0xe85250d6                ; magic number (multiboot 2 spec)
    dd 0                         ; architecture 0
    dd header_end - header_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

section .page
global pml4
global pdpe
global pde
global pdpe_low
global pte
align PAGE_SIZE
pml4:
    times 512 dq 0
pdpe:
    times 512 dq 0
pde:
    times 512 dq 0
pte:
    times 512 dq 0
pdpe_low:
    times 512 dq 0
    
align 16
GDT:
    dq 0x0000000000000000             ; Null Descriptor - should be present.
GDT.CODE:
    dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).
GDT.DATA:
    dq 0x0000920000000000             ; 64-bit data descriptor (read/write).

GDT.Pointer:
    dw $ - GDT - 1                    ; 16-bit Size (Limit) of GDT.
    dq GDT                            ; 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)

align PAGE_SIZE
STACK_END:
    times PAGE_SIZE db 0
global STACK_START
STACK_START:

align PAGE_SIZE
SMP_STACK_END:
    times PAGE_SIZE db 0
global SMP_STACK_START
SMP_STACK_START:

extern smp_callback
section .smp
global SMP_JMP
BITS 16
SMP_JMP:
    cli
    
    in al,0x92
    or al,0000_0010B
    out 0x92,al

    mov eax, 10100000b                ; Set the PAE and PGE bit.
    mov cr4, eax

    mov eax, pml4
    mov cr3, eax

    mov ecx, 0xC0000080               ; Read from the EFER MSR. 
    rdmsr    
 
    or eax, 0x00000100                ; Set the LME bit.
    wrmsr
 
    mov ebx, cr0                      ; Activate long mode -
    or ebx,0x80000001                 ; - by enabling paging and protection simultaneously.
    mov cr0, ebx

    lgdt [GDT.Pointer] 
    
    jmp CODE_SEG:smp_protect_mode
    
BITS 64
smp_protect_mode:
    ; multiple apus share the same stack
    ; we don't care because 
    mov rsp, SMP_STACK_START
    mov rbp, rsp
        
    mov rax, smp_callback
    call rax

section .boot_text
global _start
bits 32
_start:
    mov eax, pdpe_low
    or  eax, (PAGE_PRESENT | PAGE_WRITE)
    mov dword [pml4], eax

    mov eax, pdpe
    or  eax, (PAGE_PRESENT | PAGE_WRITE)
    mov dword [pml4 + 0xff8], eax

    mov eax, pde
    or  eax, (PAGE_PRESENT | PAGE_WRITE)
    mov [pdpe_low], eax

    mov eax, 0x0
    or  eax, (PAGE_PRESENT | PAGE_WRITE | PAGE_1GB)
    mov [pdpe + 0xfe0], eax
    
    mov eax, pte
    or  eax, (PAGE_PRESENT | PAGE_WRITE)
    mov [pde], eax
    
    mov edx, pte         
    mov eax, (PAGE_PRESENT | PAGE_WRITE)    
.build2MTable:
    mov [edx], eax
    add eax, 0x1000
    add edx, 8
    cmp eax, 0x200000                 ; If we did all 2MiB, end.
    jb .build2MTable

    lea eax, [pml4] 
    mov cr3, eax

    mov eax, KERNEL_CR4
    mov cr4, eax

    mov ecx, MSR_EFER
    rdmsr
    or eax, MSR_EFER_LME | MSR_EFER_SCE
    wrmsr

    mov eax, KERNEL_CR0
    mov cr0, eax

    lgdt [GDT.Pointer]

    jmp CODE_SEG:_start64

section .boot_text
bits 64
extern Kernel_Main
global _start64
_start64:
    mov ax, 0x0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rsp, STACK_START
    mov rbp, rsp

    mov rdi, rbx

    mov rax, Kernel_Main
    call rax

    cli
    hlt
loop:
    jmp loop
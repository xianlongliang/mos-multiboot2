section .text
bits 64
global syscall_entry
extern syscall_entry_c
syscall_entry:
    swapgs
    ; save userland stack
    mov [gs:8], rsp
    ; switch kernel stack
    mov rsp, [gs:0]
    ; save all

    ; save rsp, drop r15
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push rbp

    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    pushf

    mov rbp, rsp
    call syscall_entry_c
    popf
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    pop rbp
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    mov rsp, [gs:8]

    swapgs
    o64 sysret
	
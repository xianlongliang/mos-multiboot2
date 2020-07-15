
    %define _R15 0x00
    %define _R14 0x08
    %define _R13 0x10
    %define _R12 0x18
    %define _R11 0x20
    %define _R10 0x28
    %define _R9 0x30
    %define _R8 0x38

    %define _RBX 0x40
    %define _RCX 0x48
    %define _RDX 0x50
    %define _RSI 0x58
    %define _RDI 0x60
    %define _RBP 0x68
    %define _HANDLER 0x70
    %define _RAX 0x78
    %define _ISR_CODE 0x80
    %define _ERROR_CODE 0x88
    %define _RIP 0x90
    %define _CS 0x98
    %define _RFLAGS 0xa0
    %define _OLDRSP 0xa8
    %define _OLDSS 0xb0

[BITS 64]      
SECTION .text
global int_ret
int_ret:

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp

    pop rax ; handler
    pop rax ; original rax

    ;skip _ISR_CODE and _ERROR_CODE
    add rsp, 0x10

    o64 iret

global int_with_ec
int_with_ec:

    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx

    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, [rsp + _ISR_CODE]
    mov rsi, [rsp + _ERROR_CODE]
    mov rdx, [rsp + _OLDRSP]
    mov rcx, [rsp + _RFLAGS]
    mov r8,  [rsp + _RIP]
    
    mov r9, [rsp + _HANDLER]
    call r9
    jmp int_ret

; common isr_handler
; sig: void isr_handler(uint64_t isr_number);
extern isr_handler

%macro ISR_WITHOUT_ERROR_CODE 1
[GLOBAL isr%1]
isr%1:
    cli                  ; 关闭中断
    push 0               ; push error_code (invalid)
    push %1              ; push isr
    push    rax ; original rax
	lea     rax, [rel isr_handler]
    push    rax ; handler
    jmp int_with_ec
%endmacro

%macro ISR_WITH_ERROR_CODE 1
[GLOBAL isr%1]
isr%1:
    cli                  ; 关闭中断
    push    %1
    push    rax ; original rax
	lea     rax, [rel isr_handler]
    push    rax ; handler
    jmp int_with_ec
%endmacro

ISR_WITHOUT_ERROR_CODE  0    ; 0 #DE 除 0 异常
ISR_WITHOUT_ERROR_CODE  1    ; 1 #DB 调试异常
ISR_WITHOUT_ERROR_CODE  2    ; 2 NMI
ISR_WITHOUT_ERROR_CODE  3    ; 3 BP 断点异常 
ISR_WITHOUT_ERROR_CODE  4    ; 4 #OF 溢出 
ISR_WITHOUT_ERROR_CODE  5    ; 5 #BR 对数组的引用超出边界 
ISR_WITHOUT_ERROR_CODE  6    ; 6 #UD 无效或未定义的操作码 
ISR_WITHOUT_ERROR_CODE  7    ; 7 #NM 设备不可用(无数学协处理器) 
ISR_WITH_ERROR_CODE    8    ; 8 #DF 双重故障(有错误代码) 
ISR_WITHOUT_ERROR_CODE  9    ; 9 协处理器跨段操作
ISR_WITH_ERROR_CODE   10    ; 10 #TS 无效TSS(有错误代码) 
ISR_WITH_ERROR_CODE   11    ; 11 #NP 段不存在(有错误代码) 
ISR_WITH_ERROR_CODE   12    ; 12 #SS 栈错误(有错误代码) 
ISR_WITH_ERROR_CODE   13    ; 13 #GP 常规保护(有错误代码) 
ISR_WITH_ERROR_CODE   14    ; 14 #PF 页故障(有错误代码) 
ISR_WITHOUT_ERROR_CODE 15    ; 15 CPU 保留 
ISR_WITHOUT_ERROR_CODE 16    ; 16 #MF 浮点处理单元错误 
ISR_WITH_ERROR_CODE   17    ; 17 #AC 对齐检查 
ISR_WITHOUT_ERROR_CODE 18    ; 18 #MC 机器检查 
ISR_WITHOUT_ERROR_CODE 19    ; 19 #XM SIMD(单指令多数据)浮点异常
; 20 ~ 31 Intel 保留
ISR_WITHOUT_ERROR_CODE 20
ISR_WITHOUT_ERROR_CODE 21
ISR_WITHOUT_ERROR_CODE 22
ISR_WITHOUT_ERROR_CODE 23
ISR_WITHOUT_ERROR_CODE 24
ISR_WITHOUT_ERROR_CODE 25
ISR_WITHOUT_ERROR_CODE 26
ISR_WITHOUT_ERROR_CODE 27
ISR_WITHOUT_ERROR_CODE 28
ISR_WITHOUT_ERROR_CODE 29
ISR_WITHOUT_ERROR_CODE 30
ISR_WITHOUT_ERROR_CODE 31
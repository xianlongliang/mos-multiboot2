[BITS 64]
%macro IRQ 2
[GLOBAL irq%1]
irq%1:
    push 0               ; push error_code (invalid)
    push %2              ; push isr
    push    rax ; original rax
	lea     rax, [rel irq_handler]
    push    rax ; handler
    jmp int_with_ec
%endmacro
     
SECTION .text
IRQ   0,    32  ; 电脑系统计时器
IRQ   1,    33  ; 键盘
IRQ   2,    34  ; 与 IRQ9 相接，MPU-401 MD 使用
IRQ   3,    35  ; 串口设备
IRQ   4,    36  ; 串口设备
IRQ   5,    37  ; 建议声卡使用
IRQ   6,    38  ; 软驱传输控制使用
IRQ   7,    39  ; 打印机传输控制使用
IRQ   8,    40  ; 即时时钟
IRQ   9,    41  ; 与 IRQ2 相接，可设定给其他硬件
IRQ  10,    42  ; 建议网卡使用
IRQ  11,    43  ; 建议 AGP 显卡使用
IRQ  12,    44  ; 接 PS/2 鼠标，也可设定给其他硬件
IRQ  13,    45  ; 协处理器使用
IRQ  14,    46  ; IDE0 传输控制使用
IRQ  15,    47  ; IDE1 传输控制使用

extern int_ret
extern int_with_ec
extern irq_handler



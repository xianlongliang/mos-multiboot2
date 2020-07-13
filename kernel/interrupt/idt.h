#pragma once
#include <std/stdint.h>

#define INTERRUPT_MAX 48
// 定义IRQ
#define  IRQ0     32    // 电脑系统计时器
#define  IRQ1     33    // 键盘
#define  IRQ2     34    // 与 IRQ9 相接，MPU-401 MD 使用
#define  IRQ3     35    // 串口设备
#define  IRQ4     36    // 串口设备
#define  IRQ5     37    // 建议声卡使用
#define  IRQ6     38    // 软驱传输控制使用
#define  IRQ7     39    // 打印机传输控制使用
#define  IRQ8     40    // 即时时钟
#define  IRQ9     41    // 与 IRQ2 相接，可设定给其他硬件
#define  IRQ10    42    // 建议网卡使用
#define  IRQ11    43    // 建议 AGP 显卡使用
#define  IRQ12    44    // 接 PS/2 鼠标，也可设定给其他硬件
#define  IRQ13    45    // 协处理器使用
#define  IRQ14    46    // IDE0 传输控制使用
#define  IRQ15    47    // IDE1 传输控制使用

struct NO_ALIGNMENT IDT_Descriptor
{
    uint16_t offset_low;    // offset bits 0..15
    uint16_t selector;      // a code segment selector in GDT or LDT
    uint8_t istack;         // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t type_attribute; // type and attributes
    uint16_t offset_mid;    // offset bits 16..31
    uint32_t offset_high;   // offset bits 32..63
    uint32_t zero;          // reserved
};

static IDT_Descriptor idt[INTERRUPT_MAX];

struct NO_ALIGNMENT IDTR
{
    uint16_t limit;
    void *idt_address;
};

static IDTR idtr = {uint16_t(INTERRUPT_MAX * sizeof(IDT_Descriptor) - 1), idt};

typedef void (*interrupt_handler_t)(uint64_t error_code);
extern "C"
{
    void idt_init();
    void register_interrupt_handler(uint8_t n, interrupt_handler_t h);
}

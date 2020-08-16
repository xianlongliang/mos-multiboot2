#include "idt.h"
#include <std/kstring.h>
#include "gdt.h"
#include <std/printk.h>
#include <std/port_ops.h>
#include "keyboard.h"
#include "apic.h"
#include "page_fault.h"

static interrupt_handler_t interrupt_handlers[INTERRUPT_MAX] __attribute__((aligned(8)));

void IDT::Register(uint8_t n, interrupt_handler_t handler)
{
    if (!interrupt_handlers[n])
        interrupt_handlers[n] = handler;
}

static inline void load_idt(struct IDT::DescriptorPointer *idt_r)
{
    asm volatile("lidt %0" ::"m"(*idt_r));
}

extern "C"
{
    void isr0();
    void isr1();
    void isr2();
    void isr3();
    void isr4();
    void isr5();
    void isr6();
    void isr7();
    void isr8();
    void isr9();
    void isr10();
    void isr11();
    void isr12();
    void isr13();
    void isr14();
    void isr15();
    void isr16();
    void isr17();
    void isr18();
    void isr19();
    void isr20();
    void isr21();
    void isr22();
    void isr23();
    void isr24();
    void isr25();
    void isr26();
    void isr27();
    void isr28();
    void isr29();
    void isr30();
    void isr31();

    void irq0();  // 电脑系统计时器
    void irq1();  // 键盘
    void irq2();  // 与 IRQ9 相接，MPU-401 MD 使用
    void irq3();  // 串口设备
    void irq4();  // 串口设备
    void irq5();  // 建议声卡使用
    void irq6();  // 软驱传输控制使用
    void irq7();  // 打印机传输控制使用
    void irq8();  // 即时时钟
    void irq9();  // 与 IRQ2 相接，可设定给其他硬件
    void irq10(); // 建议网卡使用
    void irq11(); // 建议 AGP 显卡使用
    void irq12(); // 接 PS/2 鼠标，也可设定给其他硬件
    void irq13(); // 协处理器使用
    void irq14(); // IDE0 传输控制使用
    void irq15(); // IDE1 传输控制使用
}

#define CONVERT_ISR_ADDR(i) (uint8_t*)(&isr##i)
#define CONVERT_IRQ_ADDR(i) (uint8_t*)(&irq##i)

#define set_trap_gate(n, ist, addr) set_gate(TRAP, n, ist, addr);
#define set_intr_gate(n, ist, addr) set_gate(INTERRUPT, n, ist, addr);
#define set_system_gate(n, ist, addr) set_gate(SYSTEM, n, ist, addr);

void IDT::set_gate(IDT::DescriptorType type, unsigned int n, unsigned char ist, uint8_t*addr)
{
    uint64_t handler = reinterpret_cast<uint64_t>(addr);
    auto &id = this->idt[n];
    id.offset_low = (uint16_t)handler;
    id.selector = CODE_SEG;
    id.istack = 0;
    id.type_attribute = type;
    id.offset_mid = (uint16_t)(handler >> 16);
    id.offset_high = (uint32_t)(handler >> 32);
    id.zero = 0;
}

void IDT::Init()
{
    if (!this->inited)
    {
        this->inited = true;
        bzero(interrupt_handlers, INTERRUPT_MAX * sizeof(interrupt_handler_t));

        set_intr_gate(0, 1, CONVERT_ISR_ADDR(0));
        set_intr_gate(1, 1, CONVERT_ISR_ADDR(1));
        set_intr_gate(2, 1, CONVERT_ISR_ADDR(2));
        set_intr_gate(3, 1, CONVERT_ISR_ADDR(3));
        set_intr_gate(4, 1, CONVERT_ISR_ADDR(4));
        set_intr_gate(5, 1, CONVERT_ISR_ADDR(5));
        set_intr_gate(6, 1, CONVERT_ISR_ADDR(6));
        set_intr_gate(7, 1, CONVERT_ISR_ADDR(7));
        set_intr_gate(8, 1, CONVERT_ISR_ADDR(8));
        set_intr_gate(9, 1, CONVERT_ISR_ADDR(9));
        set_intr_gate(10, 1, CONVERT_ISR_ADDR(10));
        set_intr_gate(11, 1, CONVERT_ISR_ADDR(11));
        set_intr_gate(12, 1, CONVERT_ISR_ADDR(12));
        set_intr_gate(13, 1, CONVERT_ISR_ADDR(13));
        set_intr_gate(14, 1, CONVERT_ISR_ADDR(14));
        set_intr_gate(15, 1, CONVERT_ISR_ADDR(15));
        set_intr_gate(16, 1, CONVERT_ISR_ADDR(16));
        set_intr_gate(17, 1, CONVERT_ISR_ADDR(17));
        set_intr_gate(18, 1, CONVERT_ISR_ADDR(18));
        set_intr_gate(19, 1, CONVERT_ISR_ADDR(19));
        set_intr_gate(20, 1, CONVERT_ISR_ADDR(20));
        set_intr_gate(21, 1, CONVERT_ISR_ADDR(21));
        set_intr_gate(22, 1, CONVERT_ISR_ADDR(22));
        set_intr_gate(23, 1, CONVERT_ISR_ADDR(23));
        set_intr_gate(24, 1, CONVERT_ISR_ADDR(24));
        set_intr_gate(25, 1, CONVERT_ISR_ADDR(25));
        set_intr_gate(26, 1, CONVERT_ISR_ADDR(26));
        set_intr_gate(27, 1, CONVERT_ISR_ADDR(27));
        set_intr_gate(28, 1, CONVERT_ISR_ADDR(28));
        set_intr_gate(29, 1, CONVERT_ISR_ADDR(29));
        set_intr_gate(30, 1, CONVERT_ISR_ADDR(30));
        set_intr_gate(31, 1, CONVERT_ISR_ADDR(31));

        set_intr_gate(32, 1, CONVERT_IRQ_ADDR(0));
        set_intr_gate(33, 1, CONVERT_IRQ_ADDR(1));
        set_intr_gate(34, 1, CONVERT_IRQ_ADDR(2));
        set_intr_gate(35, 1, CONVERT_IRQ_ADDR(3));
        set_intr_gate(36, 1, CONVERT_IRQ_ADDR(4));
        set_intr_gate(37, 1, CONVERT_IRQ_ADDR(5));
        set_intr_gate(38, 1, CONVERT_IRQ_ADDR(6));
        set_intr_gate(39, 1, CONVERT_IRQ_ADDR(7));

        set_intr_gate(40, 1, CONVERT_IRQ_ADDR(8));
        set_intr_gate(41, 1, CONVERT_IRQ_ADDR(9));
        set_intr_gate(42, 1, CONVERT_IRQ_ADDR(10));
        set_intr_gate(43, 1, CONVERT_IRQ_ADDR(11));
        set_intr_gate(44, 1, CONVERT_IRQ_ADDR(12));
        set_intr_gate(45, 1, CONVERT_IRQ_ADDR(13));
        set_intr_gate(46, 1, CONVERT_IRQ_ADDR(14));
        set_intr_gate(47, 1, CONVERT_IRQ_ADDR(15));

        this->Register(14, page_fault_handler);
        this->Register(IRQ1, keyboard_irq_handler);

        /*                   ____________                          ____________
        Real Time Clock --> |            |   Timer -------------> |            |
        ACPI -------------> |            |   Keyboard-----------> |            |      _____
        Available --------> | Secondary  |----------------------> | Primary    |     |     |
        Available --------> | Interrupt  |   Serial Port 2 -----> | Interrupt  |---> | CPU |
        Mouse ------------> | Controller |   Serial Port 1 -----> | Controller |     |_____|
        Co-Processor -----> |            |   Parallel Port 2/3 -> |            |
        Primary ATA ------> |            |   Floppy disk -------> |            |
        Secondary ATA ----> |____________|   Parallel Port 1----> |____________|
        */
        // 初始化主片、从片的ICW1
        // 0001 0001
        outb(0x20, 0x11);
        outb(0xA0, 0x11);

        // 设置主片ICW2 IRQ 从 0x20(32) 号中断开始
        outb(0x21, 0x20);

        // 设置从片ICW2 IRQ 从 0x28(40) 号中断开始
        outb(0xA1, 0x28);

        // 设置主片ICW3 IR2 引脚连接从片
        outb(0x21, 0x04);

        // 设置从片ICW3 告诉从片输出引脚和主片 IR2 号相连
        outb(0xA1, 0x02);

        // 设置ICW4 设置主片和从片按照 x86 的方式工作
        outb(0x21, 0x01);
        outb(0xA1, 0x01);

        outb(0x21, 0xff);
        outb(0xA1, 0xff);
    }

    load_idt(&idtr);
    printk("IDT IDT_PTR %x\n", &idtr);
}

extern "C" void isr_handler(uint64_t isr_number, uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip)
{
    if (interrupt_handlers[isr_number])
    {
        // printk("interrupt_handlers %p\n", interrupt_handlers[isr_number]);
        interrupt_handlers[isr_number](error_code, rsp, rflags, rip);
    }
    else
    {
        // cpu_hlt();
    }
    printk("isr: %d error_code: %d\n", isr_number, error_code);
    while (isr_number != 14)
    {
        /* code */
    }
}

// static void clear_interrupt_chip(uint32_t intr_no)
// {
//     // 设置OCW3
//     // 发送中断结束信号给 PICs
//     // 按照我们的设置，从 32 号中断起为用户自定义中断
//     // 因为单片的 Intel 8259A 芯片只能处理 8 级中断
//     // 故大于等于 40 的中断号是由从片处理的
//     if (intr_no >= 40)
//     {
//         // printk("reset io_pic2");
//         // 发送重设信号给从片
//         outb(IO_PIC2, 0x20);
//     }
//     // printk("reset io_pic1");
//     // 发送重设信号给主片
//     outb(IO_PIC1, 0x20);
// }

extern "C" void irq_handler(uint64_t irq_number, uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip)
{
    if (interrupt_handlers[irq_number])
    {
        static auto apic = APIC::GetInstance();
        apic->EOI();
        interrupt_handlers[irq_number](error_code, rsp, rflags, rip);
    }
    else
    {
        panic("!interrupt_handlers[irq_number]");
    }
    // printk("irq: %d error_code: %d\n", irq_number, error_code);
}
#include "timer.h"
#include <std/port_ops.h>
#include <std/printk.h>
#include <thread/task.h>
#include "idt.h"
#include <thread/scheduler.h>

void timer_callback(uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip)
{
    static uint64_t tick = 0;
    Scheduler::GetInstance()->Schedule();
}

void timer_init(uint32_t frequency)
{
    // 注册时间相关的处理函数
    register_interrupt_handler(IRQ0, timer_callback);

    // Intel 8253/8254 PIT芯片 I/O端口地址范围是40h~43h
    // 输入频率为 1193180，frequency 即每秒中断次数
    uint32_t divisor = 1193180 / frequency;

    // D7 D6 D5 D4 D3 D2 D1 D0
    // 0  0  1  1  0  1  1  0
    // 即就是 36 H
    // 设置 8253/8254 芯片工作在模式 3 下
    outb(0x43, 0x36);

    // 拆分低字节和高字节
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);

    // 分别写入低字节和高字节
    outb(0x40, low);
    outb(0x40, hign);
}
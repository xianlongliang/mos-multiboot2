#include "pit.h"
#include <std/port_ops.h>


/* the frequency of the PIT */
#define PIT_FREQ 1193182

/* I/O ports for controlling the PIT */
#define PORT_CMD 0x0043
#define PORT_CH0 0x0040
#define PORT_CH1 0x0041
#define PORT_CH2 0x0042
#define PORT_CTRL 0x0061 /* NMI status and control port */

/* control port flags */
#define CTRL_GATE 0x01    /* gate pin for CH2 */
#define CTRL_SPEAKER 0x02 /* speaker enable */
#define CTRL_OUT 0x20     /* output pin for CH2 */

/* command port flags */
#define CMD_BINARY 0x00
#define CMD_BCD 0x01
#define CMD_MODE0 0x00 /* interrupt on terminal count */
#define CMD_MODE1 0x08 /* hardware re-triggerable one-shot */
#define CMD_MODE2 0x04 /* rate generator */
#define CMD_MODE3 0x0C /* square wave generator */
#define CMD_MODE4 0x02 /* software triggered strobe */
#define CMD_MODE5 0x0A /* hardware triggered strobe */
#define CMD_ACC_LATCH 0x00
#define CMD_ACC_LO 0x20
#define CMD_ACC_HI 0x10
#define CMD_ACC_LOHI 0x30
#define CMD_CH0 0x00
#define CMD_CH1 0x40
#define CMD_CH2 0x80
#define CMD_READBACK 0xC0

/* readback flags, lowest 6 bits are the same as the CMD bits */
#define RB_NULL 0x40
#define RB_OUT 0x80

static inline void _pit_spin(int ms)
{
    /* disable the CH2 GATE pin while we program the PIT */
    uint8_t ctrl = inb(PORT_CTRL);
    ctrl &= 0xC;
    ctrl |= 0x1;
    outb(PORT_CTRL, ctrl);

    /* send the command byte */
    // Binary mode
    // interrupt on terminal count
    // Access mode: lobyte/hibyte
    // Channel 2
    uint8_t j = CMD_BINARY | CMD_MODE0 | CMD_ACC_LOHI | CMD_CH2;
    outb(PORT_CMD, CMD_BINARY | CMD_MODE0 | CMD_ACC_LOHI | CMD_CH2);

    /* send the count word */
    uint16_t count = PIT_FREQ * ms / 1000;
    outb(PORT_CH2, count & 0xFF);
    outb(PORT_CH2, (count >> 8) & 0xFF);

    /* enable the CH2 GATE pin and ensure the speaker is disabled */
    ctrl |= CTRL_GATE;
    ctrl &= ~CTRL_SPEAKER;
    outb(PORT_CTRL, ctrl);

    /* busy-wait for the countdown to reach zero */
    while ((inb(PORT_CTRL) & CTRL_OUT) == 0)
    {
        /* also check the count ourselves */
        outb(PORT_CMD, CMD_ACC_LATCH | CMD_CH2);
        uint8_t lo = inb(PORT_CH2);
        uint8_t hi = inb(PORT_CH2);
        uint16_t count = (hi << 8) | lo;
        if (count == 0)
            break;

        /* don't burn the CPU! */
        asm volatile("pause");
    }
}

void pit_spin(int ms)
{
    /*
   * as the PIT can only count down from 65535, delays of over ~50ms overflow
   * the count register, so they are split up into multiple 50ms delays here
   */
    while (ms > 50)
    {
        _pit_spin(50);
        ms -= 50;
    }

    /* delay for any remaining time */
    if (ms > 0)
        _pit_spin(ms);
}
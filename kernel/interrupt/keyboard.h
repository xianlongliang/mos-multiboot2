#pragma once
#include <std/stdint.h>
#include <thread/io_queue.h>
#include <std/singleton.h>

void keyboard_irq_handler(uint64_t error_code, uint64_t rsp, uint64_t rflags, uint64_t rip);

class KeyboardIO : public Singleton<KeyboardIO>
{
public:
    KeyboardIO() : Queue(64) {}
    IOQueue<int8_t> Queue;
};
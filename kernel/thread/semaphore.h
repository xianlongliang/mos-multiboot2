#pragma once

#include <std/stdint.h>
#include <std/list.h>
#include <thread/task.h>
#include <std/atomic.h>

class Semaphore
{
public:
    Semaphore(uint8_t value) : value(value) {
        printk("%p\n", &this->value);
        int j = 1;
    }

    void Down();
    void Up();
private:
    atomic<uint8_t> value;
    list<task_struct *> wait_list;
};
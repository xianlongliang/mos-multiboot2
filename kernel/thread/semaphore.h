#pragma once

#include <std/stdint.h>
#include <std/list.h>
#include <thread/task.h>

class Semaphore
{
public:
    Semaphore(uint8_t value) : value(value) {}

    void Down();
    void Up();
private:
    volatile uint8_t value;
    list<task_struct *> wait_list;
};
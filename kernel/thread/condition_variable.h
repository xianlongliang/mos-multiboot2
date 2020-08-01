#pragma once

#include <std/stdint.h>
#include <std/list.h>
#include <thread/task.h>
#include <std/interrupt.h>

#include "mutex.h"

class ConditionVariable
{
public:
    ConditionVariable() {}

    void notify_one();
    void notify_all();

    template <class Predicate>
    void wait(Mutex &lock, Predicate pred)
    {
        asm volatile("pushf");
        cli();

        // task in wait_list can't wakeup by itself
        // it must be waken up by other task or interrupt
        if (!pred())
            this->wait_list.push_back(current);

        while (!pred())
        {
            lock.unlock();
            task_sleep();
            lock.lock();
        }

        asm volatile("popf");
    }

private:
    list<task_struct *> wait_list;
};
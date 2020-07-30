#include "semaphore.h"

#include <std/interrupt.h>
#include "task.h"

void Semaphore::Down()
{
    asm volatile("pushf");
    cli();

    // task in wait_list can't wakeup by itself
    // it must be waken up by other task or interrupt
    if (this->value == 0)
        this->wait_list.push_back(current);

    while (this->value == 0)
    {
        task_sleep();
    }

    this->value -= 1;

    asm volatile("popf");
}

void Semaphore::Up()
{
    asm volatile("pushf");
    cli();

    // if there's some task waiting, wake it
    if (!this->wait_list.empty())
    {
        // we don't call this->wait_list.remove(current);
        // because current must not being in the list
        // you can't wake up yourself
        auto task_to_wakeup = this->wait_list.back();
        this->wait_list.pop_back();
        task_wakeup(task_to_wakeup);
    }
    // this->value must be 0
    this->value += 1;

    asm volatile("popf");
}
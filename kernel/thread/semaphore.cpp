#include "semaphore.h"

#include <std/interrupt.h>
#include "task.h"

void Semaphore::Down()
{
    cli();

    this->wait_list.push_back(current);

    while (this->value == 0)
    {
        task_sleep();
    }

    this->value -= 1;

    sti();
}

void Semaphore::Up()
{
    cli();

    this->wait_list.remove(current);
    // if there's some task waiting, wake it
    if (!this->wait_list.empty())
    {
        auto task_to_wake = this->wait_list.back();
        task_wakeup(task_to_wake);
    }
    // this->value must be 0
    this->value += 1;

    sti();
}
#include "condition_variable.h"

void ConditionVariable::notify_one()
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

    asm volatile("popf");
}

void ConditionVariable::notify_all()
{
    asm volatile("pushf");
    cli();

    for (auto task_to_wakeup : this->wait_list)
    {
        task_wakeup(task_to_wakeup);
    }
    this->wait_list.clear();
    asm volatile("popf");
}
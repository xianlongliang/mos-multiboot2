#include "scheduler.h"
#include <std/printk.h>

extern task_struct *init_task;

void Scheduler::Schedule()
{
    auto next = this->next_task;
    this->next_task = (task_struct *)list_next(&next->list);
    // next == current only when idle task is running
    if (next == current)
        return;
    // printk("from %d to %d\n", current->pid, next->pid);
    switch_to(current, next);
}

Scheduler *Scheduler::Add(task_struct *task)
{
    if (this->next_task == nullptr)
        this->next_task = task;
    list_add_to_behind(&current->list, &task->list);
    return this;
}

Scheduler *Scheduler::Remove(task_struct *task)
{

    auto next = this->next_task = (task_struct *)list_next(&current->list);
    // if current is removed, the next task should be updated
    if (current == task)
    {
        this->next_task = next;
    }

    // if there's not task to run, add idle task back
    if (current == next)
    {
        this->next_task = init_task;
    }

    list_del(&task->list);
    return this;
}

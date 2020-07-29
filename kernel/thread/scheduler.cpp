#include "scheduler.h"
#include <std/printk.h>

void Scheduler::Schedule()
{
    auto next = (task_struct *)list_next(&current->list);
    printk("from %d to %d\n", current->pid, next->pid);
    if (next == current)
        return;
    switch_to(current, next);
}

Scheduler *Scheduler::Add(task_struct *task)
{
    if (!this->active_tasks)
        this->active_tasks = task;
    else
        list_add_to_behind(&this->active_tasks->list, &task->list);

    return this;
}

Scheduler *Scheduler::Remove(task_struct *task)
{
    if (this->active_tasks == task)
    {
        this->active_tasks = (task_struct *)list_next(&this->active_tasks->list);
    }

    list_del(&task->list);
    return this;
}

#include "scheduler.h"
#include <std/printk.h>

void Scheduler::Schedule()
{
    auto next = this->next_task;
    this->next_task = (task_struct *)list_next(&next->list);
    printk("from %d to %d\n", current->pid, next->pid);
    if (next == current)
        return;
    switch_to(current, next);
}

Scheduler *Scheduler::Add(task_struct *task)
{
    if (this->next_task == nullptr) this->next_task = task;
    list_add_to_behind(&current->list, &task->list);
    return this;
}

Scheduler *Scheduler::Remove(task_struct *task)
{
    // if current is removed, the next task should be updated
    if (current == task) {
        this->next_task = (task_struct *)list_next(&current->list);
    }
    list_del(&task->list);
    return this;
}

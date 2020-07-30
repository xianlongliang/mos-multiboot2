#pragma once

#include <std/stdint.h>
#include <std/list.h>
#include "task.h"

class Scheduler
{
public:
    static Scheduler *GetInstance()
    {
        static Scheduler instance;
        return &instance;
    }
    void Schedule();

    Scheduler* Add(task_struct* task);
    Scheduler* Remove(task_struct* task);

private:
    friend void task_init();
    task_struct *next_task;
};

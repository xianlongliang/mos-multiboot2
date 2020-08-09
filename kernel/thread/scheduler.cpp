#include "scheduler.h"
#include <std/printk.h>

extern task_struct *init_task;

void Scheduler::Schedule()
{
    if (this->next_task == nullptr)
        return;

    auto next = this->next_task;
    this->next_task = (task_struct *)list_next(&next->list);
    // next == current only when idle task is running
    if (next == current)
        return;
    // printk("from %d to %d\n", current->pid, next->pid);
    auto prev = current;
    asm volatile(
        "pushq	%rax	\n\t"
        "pushq	%rbp	\n\t"
        "pushq	%rdi	\n\t"
        "pushq	%rsi	\n\t"
        "pushq	%rdx	\n\t"
        "pushq	%rcx	\n\t"
        "pushq	%rbx	\n\t"
        "pushq	%r8	    \n\t"
        "pushq	%r9	    \n\t"
        "pushq	%r10	\n\t"
        "pushq	%r11	\n\t"
        "pushq	%r12	\n\t"
        "pushq	%r13	\n\t"
        "pushq	%r14	\n\t"
        "pushq	%r15	\n\t");

    asm volatile(
        "movq	%%rsp,	%0	\n\t"
        "movq	%2,	%%rsp	\n\t"
        "leaq	1f(%%rip),	%%rax	\n\t"
        "movq	%%rax,	%1	\n\t"
        "pushq	%3		\n\t"
        "jmp	__switch_to	\n\t"
        "1:	\n\t"
        "popq	%%r15	\n\t"
        "popq	%%r14	\n\t"
        "popq	%%r13	\n\t"
        "popq	%%r12	\n\t"
        "popq	%%r11	\n\t"
        "popq	%%r10	\n\t"
        "popq	%%r9	\n\t"
        "popq	%%r8	\n\t"
        "popq	%%rbx	\n\t"
        "popq	%%rcx	\n\t"
        "popq	%%rdx	\n\t"
        "popq	%%rdi	\n\t"
        "popq	%%rdi	\n\t"
        "popq	%%rbp	\n\t"
        "popq	%%rax	\n\t"
        : "=m"(prev->thread->rsp), "=m"(prev->thread->rip)
        : "m"(next->thread->rsp), "m"(next->thread->rip), "D"(prev), "S"(next)
        : "memory");
}

Scheduler *Scheduler::Add(task_struct *task)
{
    if (this->next_task == nullptr)
    {
        this->next_task = task;
    }
    // nothing happen when current == task
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

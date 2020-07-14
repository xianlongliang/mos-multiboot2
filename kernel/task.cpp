#include "task.h"
#include <memory/flags.h>
#include "tss.h"
#include <regs.h>
#include <std/printk.h>
#include <std/debug.h>
#include <memory/physical.h>

static mm_struct init_task_mm;
static task_struct *init_task;
static task_struct *current_task;
task_struct *get_current_task()
{
    return current_task;
}
extern "C" unsigned long do_exit(unsigned long code)
{
    printk("init2 finished\n");

    while (1)
        ;
}
static int create_kernel_thread(uint64_t (*fn)(uint64_t), uint64_t arg, uint64_t flags);

extern "C" void ret_syscall();


void schedule()
{
    auto next = list_prev(&current->list);
    auto p = (task_struct *)next;
    // printk("from %d to %d\n", current->pid, p->pid);
    switch_to(current, p);
}


uint64_t init(uint64_t arg)
{

    printk("this is init thread\n");

    // create_kernel_thread(&init2, 1, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
    // auto next = list_next(&current->list);
    // auto p = (task_struct *)next;
    // printk("current rsp : %x\n", p->thread->rsp0);
    // printk("next rsp : %x\n", p->thread->rip);
    // current_task = current;
    // switch_to(current, p);

    while (1)
    {
        printk("1");
    }
}


void task_init()
{
    // set syscall
    wrmsr(MSR_STAR, ((uint64_t)0x0020) << 48);

    auto page = PhysicalMemory::GetInstance()->Allocate(1, PG_PTable_Maped | PG_Kernel | PG_Active);

    auto init_task_stack = (void*)(Phy_To_Virt(page->physical_address) + PAGE_4K_SIZE);

    tss_struct init_task_tss = {0};
    init_task_tss.rsp0 = (uint64_t)init_task_stack;

    set_tss(init_task_tss);

    init_task = (task_struct *)Phy_To_Virt(page->physical_address);

    memset(init_task, 0, STACK_SIZE);

    list_init(&init_task->list);

    init_task->state = TASK_UNINTERRUPTIBLE;
    init_task->flags = PF_KTHREAD;
    init_task->addr_limit = 0xffff800000000000;
    init_task->pid = 0;
    init_task->counter = 1;
    init_task->signal = 0;
    init_task->priority = 0;

    // set mm and thread

    init_task->mm = &init_task_mm;
    init_task_mm.page = Get_CR3();
    init_task_mm.start_stack = init_task_stack;

    auto thread = (struct thread_struct *)(init_task + 1);
    init_task->thread = thread;
    thread->fs = KERNEL_DS;
    thread->gs = KERNEL_DS;
    thread->rsp0 = (uint64_t)init_task + STACK_SIZE;
    thread->rsp = (uint64_t)init_task + STACK_SIZE;
    thread->rip = uint64_t(&init);
    // the real stack points stack end - Regs
    init_task->state = TASK_RUNNING;

    asm volatile("movq	%0,	%%fs \n\t" ::"a"(init_task->thread->fs));
    asm volatile("movq	%0,	%%gs \n\t" ::"a"(init_task->thread->gs));
    asm volatile("movq	%0,	%%rsp \n\t" ::"a"(init_task->thread->rsp));
    asm volatile("movq	%0,	%%rbp \n\t" ::"a"(init_task->thread->rsp0));
    asm volatile("push  %0 \n\t" ::"a"(init_task->thread->rip));
    asm volatile("retq");
}

extern "C" void __switch_to(struct task_struct *prev, struct task_struct *next)
{

    auto &task_tss = get_tss();
    task_tss.rsp0 = next->thread->rsp0;
    task_tss.rsp1 = next->thread->rsp0;
    task_tss.rsp2 = next->thread->rsp0;

    set_tss(task_tss);

    __asm__ __volatile__("movq	%%fs,	%0 \n\t"
                         : "=a"(prev->thread->fs));
    __asm__ __volatile__("movq	%%gs,	%0 \n\t"
                         : "=a"(prev->thread->gs));

    __asm__ __volatile__("movq	%0,	%%fs \n\t" ::"a"(next->thread->fs));
    __asm__ __volatile__("movq	%0,	%%gs \n\t" ::"a"(next->thread->gs));

    current_task = next;

    __asm__ __volatile__("sti");
}

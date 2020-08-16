#include "task.h"
#include <memory/flags.h>
#include "tss.h"
#include <thread/regs.h>
#include <std/printk.h>
#include <std/debug.h>
#include <memory/physical.h>
#include <std/interrupt.h>
#include <memory/mapping.h>
#include <memory/kmalloc.h>
#include "scheduler.h"
#include "mutex.h"
#include "condition_variable.h"
#include <smp/cpu.h>

static uint64_t global_pid = 0;
// we pop all pt_regs out
// then restore the stack to rsp0(stack base)
// then call the fn
// then do_exit
extern "C" void kernel_thread_func();
// if not kernel thread, return to userspace
extern "C" void ret_syscall();

task_struct *init_task;

static CPU *cpus;

extern "C" unsigned long do_exit(unsigned long code)
{
    printk("init2 finished\n");

    while (1)
        ;
}

static task_struct *do_fork(struct Regs *regs, unsigned long clone_flags)
{
    auto page = PhysicalMemory::GetInstance()->Allocate(1, PG_PTable_Maped | PG_Kernel | PG_Active);

    auto task = (task_struct *)Phy_To_Virt(page->physical_address);

    memset(task, 0, sizeof(*task));
    // *task = *current;

    list_init(&task->list);
    // list_add_to_behind(&init_task->list, &task->list);

    task->pid = global_pid++;
    task->state = TASK_UNINTERRUPTIBLE;

    // place thread_struct after task_struct
    auto thread = (struct thread_struct *)(task + 1);
    bzero(thread, sizeof(struct thread_struct));
    task->thread = thread;

    regs->rsp = (uint64_t)task + STACK_SIZE;
    regs->rbp = regs->rsp;

    // copy to regs to the stack end
    memcpy((uint8_t *)((uint8_t *)task + STACK_SIZE - sizeof(Regs)), regs, sizeof(Regs));

    // stack end is equal to stack base
    thread->rsp0 = regs->rsp;
    thread->rip = regs->rip;
    // the real stack points stack end - pt_regs
    thread->rsp = (uint64_t)task + STACK_SIZE - sizeof(Regs);

    if (!(clone_flags & PF_KTHREAD))
    {
        thread->rip = (uint64_t)Phy_To_Virt(&ret_syscall);
    }

    task->state = TASK_RUNNING;

    return task;
}

static task_struct *create_kernel_thread(void (*fn)(), uint64_t arg, uint64_t flags)
{

    Regs regs;
    memset(&regs, 0, sizeof(regs));

    regs.rbx = (uint64_t)fn;

    regs.cs = KERNEL_CS;

    regs.rflags = (1 << 9); // interrupt enable
    regs.rip = (uint64_t)&kernel_thread_func;

    return do_fork(&regs, flags | PF_KTHREAD);
}

extern "C" void ret_syscall();

#include "userland.h"

// the kernel pml4
extern "C" char pml4;
extern "C" char pdpe;
extern "C" char pde;

int vmap_copy_kernel(task_struct *task)
{
    auto kernel_pml4 = (Page_PML4 *)Phy_To_Virt(&pml4);
    task->mm->pml4[511] = kernel_pml4[511];
}

void userland_page_init(task_struct *task)
{
    vmap_frame(current, 0x400000, 0x07);
    vmap_copy_kernel(current);
    task->mm->start_code = (uint8_t *)0x400000;
    task->mm->end_code = (uint8_t *)0x400000 + PAGE_4K_SIZE;
}

void init2()
{
    int i = 0;
    while (1)
    {
        ++i;
        // printk("%d", current->pid);
    }
    cli();
    printk("this is init 2\n");

    auto task = current;
    task->mm = (mm_struct *)((char *)task->thread + sizeof(thread_struct));
    bzero(task->mm, sizeof(mm_struct));
    userland_page_init(task);
    set_cr3(Virt_To_Phy(task->mm->pml4));
    memcpy((uint8_t *)task->mm->start_code, (uint8_t *)&userland_entry, 1024);
    auto ret_syscall_addr = uint64_t(&ret_syscall);
    auto ret_stack = uint64_t((uint8_t *)task + STACK_SIZE - sizeof(Regs));
    asm volatile("movq	%0,	%%rsp	\n\t"
                 "pushq	%1		    \n\t" ::
                     "m"(ret_stack),
                 "m"(ret_syscall_addr)
                 : "memory");

    auto regs = (Regs *)ret_stack;
    bzero(regs, sizeof(Regs));
    regs->rsp = (uint64_t)(0x402fff & 0xfffff0); // RSP
    regs->rbp = regs->rsp;                       // RSP
    regs->rcx = (uint64_t)(0x400000);            // RIP
    regs->r11 = (1 << 9);

    printk("enter userland\n");

    asm volatile("retq");
}

extern "C" ssize_t sys_read(int fd, uint8_t *buf, size_t count);
extern "C" ssize_t sys_write(int fd, uint8_t *buf, size_t count);

void bash()
{
    printk("mos-kernel# ");
    int8_t bash_buffer[1024];
    uint64_t bash_buffer_cur = 0;
    while (1)
    {
        uint8_t ch = 0;
        sys_read(1, &ch, 1);
        switch (ch)
        {
        case '\r':
            if (strcmp(bash_buffer, "clear") == 0)
            {
                clear();
            }
            else if (strcmp(bash_buffer, "help") == 0)
                printk("\nmos kernel v0.0.1\n");
            else if (bash_buffer_cur != 0)
            {
                printk("\n%s: command not found\n", bash_buffer);
            }
            else
            {
                printk("\n");
            }
            printk("mos-kernel# ");
            bzero(bash_buffer, bash_buffer_cur);
            bash_buffer_cur = 0;
            break;
        case '\b':
        {
            if (bash_buffer_cur == 0)
                break;
            bash_buffer[--bash_buffer_cur] = 0;
            sys_write(1, &ch, 1);
            break;
        }
        default:
        {
            bash_buffer[bash_buffer_cur++] = ch;
            sys_write(1, &ch, 1);
        }
        }
    }
}

uint64_t init(uint64_t arg)
{
    printk("this is kernel thread\n");

    auto task_init2 = create_kernel_thread(&init2, 1, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
    auto bash_task = create_kernel_thread(&bash, 1, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
    printk("current rsp : %x\n", current->thread->rsp0);
    printk("task_init2 rsp : %x\n", task_init2->thread->rsp0);

    cpus->Get().scheduler.Add(current)->Add(bash_task);

    // switch_to(current, next);
    // sti();
    while (1)
    {
        // remove self from the scheduler when there are others tasks running
        asm volatile("sti; hlt");
        // idle task will be added back when there's no task to run
    }
}

void task_init()
{
    cpus = CPU::GetInstance();

    auto page = PhysicalMemory::GetInstance()->Allocate(1, PG_PTable_Maped | PG_Kernel | PG_Active);

    auto init_task_stack = (uint8_t *)(Phy_To_Virt(page->physical_address) + PAGE_4K_SIZE);

    tss_struct init_task_tss;
    bzero(&init_task_tss, sizeof(tss_struct));
    init_task_tss.rsp0 = (uint64_t)init_task_stack;

    set_tss(init_task_tss);

    init_task = (task_struct *)Phy_To_Virt(page->physical_address);

    memset(init_task, 0, STACK_SIZE);

    list_init(&init_task->list);

    init_task->state = TASK_UNINTERRUPTIBLE;
    init_task->flags = PF_KTHREAD;
    init_task->pid = global_pid++;
    init_task->signal = 0;
    init_task->priority = 0;

    // set mm and thread

    init_task->mm = nullptr;

    auto thread = (struct thread_struct *)(init_task + 1);
    init_task->thread = thread;
    thread->fs = KERNEL_DS;
    thread->gs = KERNEL_DS;
    thread->rsp0 = (uint64_t)init_task + STACK_SIZE;
    thread->rsp = (uint64_t)init_task + STACK_SIZE - sizeof(Regs) - 0x8;
    thread->rip = uint64_t(&init);
    // the real stack points stack end - Regs
    init_task->state = TASK_RUNNING;

    asm volatile("movq  %0, %%r15   \n\t"  ::"a"(init_task->thread->rip));
    asm volatile("movq	%0,	%%fs  \n\t"  ::"a"(init_task->thread->fs));
    asm volatile("movq	%0,	%%gs  \n\t"  ::"a"(init_task->thread->gs));
    asm volatile("movq	%0,	%%rsp \n\t" ::"a"(init_task->thread->rsp));
    asm volatile("movq	%0,	%%rbp \n\t" ::"a"(init_task->thread->rsp0));
    asm volatile("push  %r15 \n\t");
    asm volatile("retq");
}

extern "C" void __switch_to(struct task_struct *prev, struct task_struct *next)
{

    auto &task_tss = get_tss();
    task_tss.rsp0 = next->thread->rsp0;
    set_tss(task_tss);

    asm volatile("movq	%%fs,	%0 \n\t"
                 : "=a"(prev->thread->fs));
    asm volatile("movq	%%gs,	%0 \n\t"
                 : "=a"(prev->thread->gs));

    asm volatile("movq	%0,	%%fs \n\t" ::"a"(next->thread->fs));
    asm volatile("movq	%0,	%%gs \n\t" ::"a"(next->thread->gs));

    if (prev->mm == nullptr && next->mm)
    {
        // printk("kernel to userland\n");
        set_cr3(Virt_To_Phy(next->mm->pml4));
        flush_tlb();
    }
    else if (prev->mm && next->mm == nullptr)
    {
        // printk("userland to kernel\n");
        set_cr3(&pml4);
        flush_tlb();
    }
}

void task_sleep()
{
    asm volatile("pushf");
    asm volatile("cli");
    current->state = TASK_STOPPED;
    cpus->Get().scheduler.Remove(current)->Schedule();
    asm volatile("popf");
}

void task_yield()
{
    asm volatile("pushf");
    asm volatile("cli");
    cpus->Get().scheduler.Schedule();
    asm volatile("popf");
}

void task_wakeup(task_struct *task)
{
    asm volatile("pushf");
    asm volatile("cli");
    task->state = TASK_RUNNING;
    cpus->Get().scheduler.Add(task);
    asm volatile("popf");
}
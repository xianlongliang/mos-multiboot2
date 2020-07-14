#pragma once
#include <std/stdint.h>
#include <std/list.h>
#include <tss.h>

//GDT selector
#define KERNEL_CS (0x08)
#define KERNEL_DS (0x10)
#define USER_CS (0x0030)
#define USER_DS (0x28)

#define PF_KTHREAD (1 << 0)

// task states
#define TASK_RUNNING (1 << 0)
#define TASK_INTERRUPTIBLE (1 << 1)
#define TASK_UNINTERRUPTIBLE (1 << 2)
#define TASK_ZOMBIE (1 << 3)
#define TASK_STOPPED (1 << 4)

// options for creating task
#define CLONE_FS (1 << 0)
#define CLONE_FILES (1 << 1)
#define CLONE_SIGNAL (1 << 2)

struct mm_struct
{
    void* page; //page table point

    // all addresses below are virtual
    void* start_code;
    void* end_code;
    void* start_data;
    void* end_data;
    void* start_rodata;
    void* end_rodata;
    void* start_brk;
    void* end_brk;
    void* start_stack;
};

struct thread_struct
{
    // kernel base stack
    // also saved in tss
    uint64_t rsp0;

    // kernel rip
    uint64_t rip;
    // kernel rsp
    uint64_t rsp;

    uint64_t fs;
    uint64_t gs;

    uint64_t cr2;
    uint64_t trap_nr;
    uint64_t error_code;
};

struct task_struct
{
    List list;
    volatile uint8_t state;
    uint8_t flags;

    mm_struct *mm;
    thread_struct *thread;

    uint64_t pid;
    uint64_t signal;
    uint64_t priority;
};

constexpr uint64_t STACK_SIZE = 4096;

void task_init();
task_struct *get_current_task();

inline struct task_struct *get_current()
{
    struct task_struct *current = nullptr;
    __asm__ __volatile__("andq %%rsp,%0	\n\t"
                         : "=r"(current)
                         : "0"(~4095UL));
    return current;
}

#define current get_current()

// params rdi, rsi
#define switch_to(prev, next)                                                      \
    do                                                                             \
    {                                                                              \
        __asm__ __volatile__(                                                      \
            "pushq	%%rax	\n\t"                                                     \
            "pushq	%%rbp	\n\t"                                                     \
            "pushq	%%rdi	\n\t"                                                     \
            "pushq	%%rsi	\n\t"                                                     \
            "pushq	%%rdx	\n\t"                                                     \
            "pushq	%%rcx	\n\t"                                                     \
            "pushq	%%rbx	\n\t"                                                     \
            "pushq	%%r8	\n\t"                                                      \
            "pushq	%%r9	\n\t"                                                      \
            "pushq	%%r10	\n\t"                                                     \
            "pushq	%%r11	\n\t"                                                     \
            "pushq	%%r12	\n\t"                                                     \
            "pushq	%%r13	\n\t"                                                     \
            "pushq	%%r14	\n\t"                                                     \
            "pushq	%%r15	\n\t"                                                     \
            "movq	%%rsp,	%0	\n\t"                                                  \
            "movq	%2,	%%rsp	\n\t"                                                  \
            "leaq	1f(%%rip),	%%rax	\n\t"                                           \
            "movq	%%rax,	%1	\n\t"                                                  \
            "pushq	%3		\n\t"                                                       \
            "jmp	__switch_to	\n\t"                                                 \
            "1:	\n\t"                                                              \
            "popq	%%r15	\n\t"                                                      \
            "popq	%%r14	\n\t"                                                      \
            "popq	%%r13	\n\t"                                                      \
            "popq	%%r12	\n\t"                                                      \
            "popq	%%r11	\n\t"                                                      \
            "popq	%%r10	\n\t"                                                      \
            "popq	%%r9	\n\t"                                                       \
            "popq	%%r8	\n\t"                                                       \
            "popq	%%rbx	\n\t"                                                      \
            "popq	%%rcx	\n\t"                                                      \
            "popq	%%rdx	\n\t"                                                      \
            "popq	%%rdi	\n\t"                                                      \
            "popq	%%rdi	\n\t"                                                      \
            "popq	%%rbp	\n\t"                                                      \
            "popq	%%rax	\n\t"                                                      \
            : "=m"(prev->thread->rsp), "=m"(prev->thread->rip)                     \
            : "m"(next->thread->rsp), "m"(next->thread->rip), "D"(prev), "S"(next) \
            : "memory");                                                           \
    } while (0)

extern "C" void schedule();

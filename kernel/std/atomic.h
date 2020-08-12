#pragma once

typedef enum memory_order
{
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;

template <class T>
class atomic
{

public:
    atomic() : val(0) {}
    atomic(T v) : val(v) {}

    void store(T v, memory_order order = memory_order_seq_cst)
    {
        switch (order)
        {
        case memory_order_seq_cst:
        {
            asm volatile("movq %1, %0 \n\t"
                         "mfence \n\t"
                         : "=m"(this->val)
                         : "r"((uint64_t)v)
                         : "memory");
            break;
        }
        default:
        {
            asm volatile("movq %1, %0 \n\t"
                         : "=m"(this->val)
                         : "r"((uint64_t)v)
                         : "memory");
            break;
        }
        }
    }

    T load()
    {
        uint64_t tmp;
        asm volatile("movq %1, %%rdi \n\t"
                     "movq %%rdi, %0 \n\t"
                     : "=m"(tmp)
                     : "m"(this->val)
                     : "memory", "rdi");
        return (T)tmp;
    }

    T fetch_add(T v)
    {
        asm volatile("lock; addq %1, %0 \n\t"
                     : "=m"(this->val)
                     : "r"((uint64_t)v)
                     : "memory");
    }

    T fetch_sub(T v)
    {
        asm volatile("lock; subq %1, %0 \n\t"
                     : "=m"(this->val)
                     : "r"((uint64_t)v)
                     : "memory");
    }

    bool operator==(T v)
    {
        return this->load() == v;
    }

    void operator+=(T v)
    {
        this->fetch_add(v);
    }

    void operator-=(T v)
    {
        this->fetch_sub(v);
    }

private:
    alignas(T) T val;
};
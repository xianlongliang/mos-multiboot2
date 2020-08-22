#pragma once

template <class T>
class atomic
{
public:
    void store(T v)
    {
        asm volatile("movq %1, %%rax \n\t"
                     "movq %%rax, %0 \n\t"
                     "mfence \n\t"
                     : "=m"(this->val)
                     : "m"(v)
                     : "memory", "rax");
    }

    T load()
    {
        T tmp;
        asm volatile("movq %1, %%rdi \n\t"
                     "movq %%rdi, %0 \n\t"
                     : "=m"(tmp)
                     : "m"(this->val)
                     : "memory", "rdi");
        return tmp;
    }

    bool operator==(T v)
    {
        return this->load() == v;
    }

    bool compare_exchange(T &old_val, T new_val)
    {
        return __sync_bool_compare_and_swap((uint64_t *)&this->val, *((uint64_t *)&old_val), *((uint64_t *)&new_val));
    }

private:
    alignas(T) T val;
};

template <class T>
struct atomic_number
{
    atomic_number() : val(0) {}
    atomic_number(T v) : val(v) {}

    T fetch_add(T v)
    {
        asm volatile("lock; addq %1, %0 \n\t"
                     : "=m"(this->val)
                     : "r"((uint64_t)v)
                     : "memory");
        return (T)this->val;
    }

    T fetch_sub(T v)
    {
        asm volatile("lock; subq %1, %0 \n\t"
                     : "=m"(this->val)
                     : "r"((uint64_t)v)
                     : "memory");
        return (T)this->val;
    }

    void operator+=(T v)
    {
        this->fetch_add(v);
    }

    void operator-=(T v)
    {
        this->fetch_sub(v);
    }
    void store(T v)
    {
        asm volatile("movq %1, %0 \n\t"
                     "mfence \n\t"
                     : "=m"(this->val)
                     : "r"((uint64_t)v)
                     : "memory");
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

    bool operator==(T v)
    {
        return this->load() == v;
    }

    bool compare_exchange(T &old_val, T new_val)
    {
        return __sync_bool_compare_and_swap(this->val, old_val, new_val);
    }

private:
    alignas(T) T val;
};

template <>
struct atomic<uint8_t> : public atomic_number<uint8_t>
{
    atomic() noexcept = default;
    ~atomic() noexcept = default;
    atomic(const atomic &) = delete;
    atomic &operator=(const atomic &) = delete;
    atomic &operator=(const atomic &) volatile = delete;

    atomic(uint8_t val) noexcept : atomic_number(val) {}
};

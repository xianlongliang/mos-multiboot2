#pragma once

namespace std
{
    // for custom type
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

        bool compare_exchange(T &old_val, T new_val) requires(sizeof(T) == 1)
        {
            return __sync_bool_compare_and_swap_1((uint8_t *)&this->val, *((uint8_t *)&old_val), *((uint8_t *)&new_val));
        }

        bool compare_exchange(T &old_val, T new_val) requires(sizeof(T) == 2)
        {
            return __sync_bool_compare_and_swap_2((uint16_t *)&this->val, *((uint16_t *)&old_val), *((uint16_t *)&new_val));
        }

        bool compare_exchange(T &old_val, T new_val) requires(sizeof(T) == 4)
        {
            return __sync_bool_compare_and_swap_4((uint32_t *)&this->val, *((uint32_t *)&old_val), *((uint32_t *)&new_val));
        }

        bool compare_exchange(T &old_val, T new_val) requires(sizeof(T) == 8)
        {
            return __sync_bool_compare_and_swap_8((uint64_t *)&this->val, *((uint64_t *)&old_val), *((uint64_t *)&new_val));
        }

        bool compare_exchange(T &old_val, T new_val) requires(sizeof(T) == 16)
        {
            return __sync_bool_compare_and_swap_16((uint128_t *)&this->val, *((uint128_t *)&old_val), *((uint128_t *)&new_val));
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

        T operator++() {
            return this->fetch_add(1);
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

    private:
        alignas(T) T val = 0;
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

    template <>
    struct atomic<uint128_t> : public atomic_number<uint128_t>
    {
        atomic() noexcept = default;
        ~atomic() noexcept = default;
        atomic(const atomic &) = delete;
        atomic &operator=(const atomic &) = delete;
        atomic &operator=(const atomic &) volatile = delete;

        atomic(uint128_t val) noexcept : atomic_number(val) {}
    };

    template <>
    struct atomic<uint64_t> : public atomic_number<uint64_t>
    {
        atomic() noexcept = default;
        ~atomic() noexcept = default;
        atomic(const atomic &) = delete;
        atomic &operator=(const atomic &) = delete;
        atomic &operator=(const atomic &) volatile = delete;

        atomic(uint64_t val) noexcept : atomic_number(val) {}
    };
}; 
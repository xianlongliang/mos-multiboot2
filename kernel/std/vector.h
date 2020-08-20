#pragma once

#include "new.h"
#include "move.h"
#include "debug.h"
#include "printk.h"

template <typename T>
class vector
{

public:
    vector() {}

    vector(uint64_t capacity) : capacity(capacity), current_size(0)
    {
        this->pval = (T *)kmalloc(sizeof(T) * capacity, 0);
    }

    vector(uint64_t capacity, const T& value) : capacity(capacity)
    {
        this->pval = (T *)kmalloc(sizeof(T) * capacity, 0);
        for (uint64_t i = 0; i < capacity; ++i) {
            new (&this->pval[i]) T(value);
        }
    }

    vector(vector&& rval) {
        this->pval = rval.pval;
        rval.pval = nullptr;
        this->current_size = rval.current_size;
        rval.current_size = 0;
        this->capacity = rval.capacity;
        rval.capacity = 0;
    }

    ~vector() {
        while(!this->empty()) {
            this->pop_back();
        }
        delete this->pval;
    }

    vector& operator=(vector&& rval) {
        swap(this->pval, rval.pval);
        swap(this->current_size, rval.current_size);
        swap(this->capacity, rval.capacity);
        return *this;
    }

    bool empty()
    {
        return this->current_size == 0;
    }

    void push_back(T &&val)
    {
        if (this->current_size == this->capacity)
        {
            this->expand();
        }
        new (&this->pval[this->current_size++]) T(move(val));
    }

    void pop_back()
    {
        if (this->empty())
            panic("pop_back empty vector");

        this->pval[this->current_size--].~T();
    }

    T &back()
    {
        return this->pval[this->current_size - 1];
    }

    void push_front(T &&val);

    void pop_front()
    {
        if (this->empty())
            panic("pop_front empty vector");
    }

    uint64_t size()
    {
        return this->current_size;
    }

    void clear()
    {
        while (!this->empty())
        {
            auto back = this->back();
            this->pop_back();
            delete back;
        }
    }

    T& operator[](uint64_t index) {
        return this->pval[index];
    }
    // // Minimum required for range-for loop
    // template <typename IT>
    // struct Iterator
    // {
    //     IT *p;
    //     T &operator*() { return p->val; }
    //     bool operator!=(const Iterator &rhs)
    //     {
    //         return p != rhs.p;
    //     }
    //     void operator++() { p = p->next; }
    // };

    // // auto return requires C++14
    // auto begin() const
    // { // const version
    //     return Iterator<list_node>{this->head->next};
    // }

    // auto end() const
    // { // const version
    //     return Iterator<list_node>{this->head};
    // }

private:
    T *pval;
    uint64_t current_size;
    uint64_t capacity;

    void expand()
    {
        uint64_t target_capacity = capacity * 2;
        if (target_capacity == 0)
            target_capacity = 1;
        auto new_pval = (T *)kmalloc(target_capacity * sizeof(T), 0);
        for (uint64_t i = 0; i < this->current_size; ++i)
        {
            new (&new_pval[i]) T(move(this->pval[i]));
        }
        if (this->pval) kfree(this->pval);
        this->pval = new_pval;
        this->capacity = target_capacity;
    }
};
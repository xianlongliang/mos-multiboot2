#pragma once

#include "new.h"
#include "move.h"
#include "debug.h"
#include "printk.h"

template <typename T>
class Set
{
    struct list_node
    {
        list_node() {}
        list_node(T &&val) : val(val), next(nullptr), prev(nullptr) {}
        T val;
        list_node *next;
        list_node *prev;
    };

public:
    list()
    {
        this->head = new list_node();
        this->head->next = this->head;
        this->head->prev = this->head;
        this->list_size = 0;
    }

    bool empty()
    {
        return (this->head->next == this->head) && (this->head->prev == this->head);
    }

    void remove(T val)
    {
        if (this->empty())
            panic("remove empty list");

        auto node = this->head->next;
        auto end = this->head;
        while (node != this->head)
        {
            if (node->val == val)
                break;
            node = node->next;
        }
        // if not found
        if (node == end)
            return;

        node->prev->next = node->next;
        node->next->prev = node->prev;

        delete node;

        this->list_size--;
    }

    void push_back(T &&val)
    {
        auto node = new list_node(forward<T>(val));
        auto last = head->prev;
        head->prev = node;
        last->next = node;
        node->prev = last;
        node->next = head;
        this->list_size++;
    }

    void pop_back()
    {
        if (this->empty())
            panic("pop_back empty list");

        auto node_to_pop = head->prev;
        node_to_pop->prev->next = head;
        head->prev = node_to_pop->prev;
        delete node_to_pop;
        this->list_size--;
    }

    T &back()
    {
        if (!head)
            panic("back empty list");

        return head->prev->val;
    }

    void push_front(T &&val)
    {
        auto node = new list_node(forward<T>(val));
        this->head->next->prev = node;
        node->next = this->head->next;
        node->prev = this->head;
        this->head->next = node;
        this->list_size++;
    }

    void pop_front()
    {
        if (this->empty())
            panic("pop_front empty list");

        auto node_to_pop = head->next;

        this->head->next = node_to_pop->next;
        node_to_pop->next->prev = this->head;

        delete node_to_pop;
        this->list_size--;
    }

    uint64_t size()
    {
        return this->list_size;
    }

    void clear() {
        while(!this->empty()) {
            auto back = this->back();
            this->pop_back();
            delete back;
        }
    }

    // Minimum required for range-for loop
    template <typename IT>
    struct Iterator
    {
        IT *p;
        T &operator*() { return p->val; }
        bool operator!=(const Iterator &rhs)
        {
            return p != rhs.p;
        }
        void operator++() { p = p->next; }
    };

    // auto return requires C++14
    auto begin() const
    { // const version
        return Iterator<list_node>{this->head->next};
    }

    auto end() const
    { // const version
        return Iterator<list_node>{this->head};
    }

private:
    list_node *head;
    uint64_t list_size;
};
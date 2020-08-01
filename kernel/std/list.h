#pragma once

#include "new.h"
#include "move.h"
#include "debug.h"
#include "printk.h"

#define container_of(ptr, type, member)                                     \
    ({                                                                      \
        typeof(((type *)0)->member) *p = (ptr);                             \
        (type *)((unsigned long)p - (unsigned long)&(((type *)0)->member)); \
    })

struct List
{
    struct List *prev;
    struct List *next;
};

inline void list_init(struct List *list)
{
    list->prev = list;
    list->next = list;
}

inline void list_add_to_behind(struct List *entry, struct List *node) ////add to entry behind
{
    node->next = entry->next;
    node->prev = entry;
    node->next->prev = node;
    entry->next = node;
}

inline void list_add_to_before(struct List *entry, struct List *node) ////add to entry behind
{
    node->next = entry;
    entry->prev->next = node;
    node->prev = entry->prev;
    entry->prev = node;
}

inline void list_del(struct List *entry)
{
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    list_init(entry);
}

inline long list_is_empty(struct List *entry)
{
    if (entry == entry->next && entry->prev == entry)
        return 1;
    else
        return 0;
}

inline struct List *list_prev(struct List *entry)
{
    if (entry->prev != nullptr)
        return entry->prev;
    else
        return nullptr;
}

inline struct List *list_next(struct List *entry)
{
    if (entry->next != nullptr)
        return entry->next;
    else
        return nullptr;
}

template <typename T>
class list
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
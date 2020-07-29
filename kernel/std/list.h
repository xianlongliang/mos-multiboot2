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
        list_node(T &&val) : val(val), next(nullptr), prev(nullptr) {}
        T val;
        list_node *next;
        list_node *prev;
    };

public:
    list() : head(nullptr) {}

    bool empty()
    {
        return this->head == nullptr;
    }

    void remove(T val)
    {
        if (!head)
            panic("remove empty list");

        auto p = this->head;
        do
        {
            if (p->val == val)
                break;
            p = p->next;
        } while (p != this->head);

        // if there's only node in list
        if (p->next == p)
        {
            delete p;
            this->head == nullptr;
        }
        p->prev->next = p->next;
        p->next->prev = p->prev;
        if (p == this->head) this->head = p->next;
        delete p;
    }

    void push_back(T &&val)
    {
        auto node = new list_node(forward<T>(val));
        if (head == nullptr)
        {
            head = node;
            head->prev = head->next = head;
        }
        else
        {
            auto last = head->prev;
            head->prev = node;
            last->next = node;
            node->prev = last;
            node->next = head;
        }
    }

    void pop_back()
    {
        if (!head)
            panic("pop empty list");

        auto node_to_pop = head->prev;
        if (head == node_to_pop)
        {
            head = nullptr;
        }
        else
        {
            node_to_pop->prev->next = head;
            head->prev = node_to_pop->prev;
        }

        delete node_to_pop;
    }

    T &back()
    {
        if (!head)
            panic("back empty list");

        return head->prev->val;
    }

private:
    list_node *head;
};
#pragma once

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
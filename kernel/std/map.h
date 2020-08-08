#pragma once

#include "avl_tree.h"

template<class T>
class map {
    public:

    bool empty() {
        return this->tree.empty();
    }

    uint64_t size();

    void insert(uint64_t key, T&& val) {
        auto pval = new T(move(val));
        this->tree.insert(key, pval);
    }

    T* find(uint64_t key) {
        auto node = this->tree.find(key);
        if (node == nullptr) return nullptr;
        return node->val;
    }

    void erase(uint64_t key) {
        this->tree.erase(key);
    }

private: 
    AVLTree<T> tree;
};
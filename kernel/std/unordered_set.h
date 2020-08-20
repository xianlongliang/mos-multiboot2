#pragma once

#include <std/list.h>
#include <std/vector.h>

template <class T>
uint64_t hash(T &key);

template<>
uint64_t hash(uint64_t& key) { return key; }

template<>
uint64_t hash(int& key) { return key; }

template <class T>
class unordered_set
{

public:

    unordered_set() : current_bucket_size_prime_idx(4), load_count(0), buckets(primes_table[4])
    {
        for (int i = 0; i < primes_table[this->current_bucket_size_prime_idx]; ++i) {
            this->buckets.push_back(list<T>());
        }
    }

    ~unordered_set()
    {
    }

    bool insert(T &key)
    {
        return this->insert(T(key));
    }

    bool insert(T &&key)
    {
        if (this->load_factor() >= 1.0f)
        {
            this->expand();
        }

        if (find(key)) return false;

        auto hash_idx = hash(key);
        list<T>& list = this->buckets[hash_mod(hash_idx)];
        list.push_back(move(key));
        load_count++;
        return true;
    }

    T* find(T& key) {
        auto hash_idx = hash(key);
        list<T>& list = this->buckets[hash_mod(hash_idx)];
        for (auto& node : list) {
            if (node == key) return &node;
        }
        return nullptr;
    }

    bool empty();

private:
    uint32_t current_bucket_size_prime_idx;
    uint32_t load_count;

    // dynamic array of bucket_list_node*, pointing to the list node
    vector<list<T>> buckets;

    inline float load_factor() {
        return float(this->load_count) / float(primes_table[this->current_bucket_size_prime_idx]);
    }

    inline uint64_t hash_mod(uint64_t hash) {
        return hash % primes_table[this->current_bucket_size_prime_idx];
    }

    void expand()
    {
        auto old_bucket_size = primes_table[this->current_bucket_size_prime_idx];
        this->current_bucket_size_prime_idx++;
        auto new_bucket_size = primes_table[this->current_bucket_size_prime_idx];
        auto new_buckets = vector<list<T>>(new_bucket_size);
        // init buckets
        for (int i = 0; i < primes_table[this->current_bucket_size_prime_idx]; ++i) {
            new_buckets.push_back(list<T>());
        }

        for (uint64_t i = 0; i < old_bucket_size; ++i)
        {
            auto& p = this->buckets[i];
            while (!p.empty())
            {
                auto back = p.back();
                p.pop_back();
                auto hash_idx = hash(back);
                auto idx = hash_mod(hash_idx);
                new_buckets[hash_mod(hash_idx)].push_back(move(back));
            }
        }

        this->buckets = move(new_buckets);
    }
};
#pragma once

#include <std/stdint.h>
#include <std/new.h>
#include <std/string.h>
#include <std/move.h>

class string
{
public:
    string() : len(0)
    {
        this->ptr = string::zero_str;
        this->len = 0;
    }

    string(char *str, uint64_t len)
    {
        this->len = len;
        this->ptr = new char[this->len + 1];
        memcpy(this->ptr, str, this->len);
        this->ptr[this->len] = '\0';
    }

    string(char *str) : string(str, strlen(str)) {}

    string(const string &other) : string(other.ptr) {}

    string &operator=(string other)
    {
        std::swap(this->ptr, other.ptr);
        std::swap(this->len, other.len);
        return *this;
    }

    string(string &&other) : ptr(other.ptr), len(other.len)
    {
        other.ptr = nullptr;
        other.len = 0;
    }

    ~string()
    {
        this->len = 0;
        if (this->ptr != string::zero_str)
            delete this->ptr;
    }

    char operator[](int index) const
    {
        return this->ptr[index];
    }

    void operator+=(const string &other)
    {
        auto len = this->len + other.len;
        auto p = new char[len + 1];
        memcpy(p, this->ptr, this->len);
        memcpy(p + this->len, other.ptr, other.len);
        p[len] = '\0';

        kfree(this->ptr);
        this->ptr = p;
        this->len = len;
    }

    string operator+(const string &other)
    {
        string tmp = *this;
        tmp += other;
        return tmp;
    }

    uint64_t length() { return this->len; }

    char *c_str() { return this->ptr; }

private:
    inline static char zero_str[1]  = {'\0'};
    uint64_t len;
    char *ptr;
};
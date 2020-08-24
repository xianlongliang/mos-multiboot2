#pragma once

template <typename T>
void swap(T &a, T &b) noexcept
{
    auto tmp = a;
    a = b;
    b = tmp;
}

template <typename T>
struct remove_reference
{
    typedef T type;
};

template <typename T>
struct remove_reference<T &>
{
    typedef T type;
};

template <typename T>
struct remove_reference<T &&>
{
    typedef T type;
};

/*
    reference collapsing rules here:
    
    TR   R
    T&   &  -> T&  // lvalue reference to cv TR -> lvalue reference to T
    T&   && -> T&  // rvalue reference to cv TR -> TR (lvalue reference to T)
    T&&  &  -> T&  // lvalue reference to cv TR -> lvalue reference to T
    T&&  && -> T&& // rvalue reference to cv TR -> TR (rvalue reference to T)
*/
template <typename T>
constexpr T &&
forward(typename remove_reference<T>::type &t) noexcept
{
    return static_cast<T &&>(t);
}

template <typename T>
constexpr T &&
forward(typename remove_reference<T>::type &&t) noexcept
{
    return static_cast<T &&>(t);
}

template <typename T>
constexpr typename remove_reference<T>::type &&
move(T &&t) noexcept
{
    return static_cast<typename remove_reference<T>::type &&>(t);
}

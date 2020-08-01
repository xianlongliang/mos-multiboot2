#pragma once

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
    // static_assert(!std::is_lvalue_reference<T>::value, "template argument"
    //                                                      " substituting T is an lvalue reference type");
    return static_cast<T &&>(t);
}

template <typename _Tp>
constexpr typename remove_reference<_Tp>::type &&
move(_Tp &&__t) noexcept
{
    return static_cast<typename remove_reference<_Tp>::type &&>(__t);
}

#pragma once

#include <memory>


#if __cplusplus <= 201103L
namespace std
{
//template<typename T, typename... Args>
//std::unique_ptr<T> make_unique(Args&&... args)
//{
//    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
//}

template<typename T>
std::unique_ptr<T> make_unique()
{
    return std::unique_ptr<T>(new T);
}
template<typename T, typename Arg1>
std::unique_ptr<T> make_unique(Arg1&& arg1)
{
    return std::unique_ptr<T>(new T(std::forward<Arg1>(arg1)));
}
template<typename T, typename Arg1, typename Arg2>
std::unique_ptr<T> make_unique(Arg1&& arg1, Arg2&& arg2)
{
    return std::unique_ptr<T>(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)));
}
template<typename T, typename Arg1, typename Arg2, typename Arg3>
std::unique_ptr<T> make_unique(Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
{
    return std::unique_ptr<T>(new T(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2), std::forward<Arg3>(arg3)));
}
}

#ifndef _MSC_VER
#define NOEXCEPT noexcept
#define NOEXCEPTFALSE noexcept(false)
#else
#define NOEXCEPT
#define NOEXCEPTFALSE
#endif

#endif // __cplusplus <= 201103L

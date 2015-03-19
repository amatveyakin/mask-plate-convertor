#pragma once

#include <memory>


#if __cplusplus <= 201103L
namespace std
{
#ifndef _MSC_VER  // MSVC 2013 defines make_unique anyway
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif
}

#endif // __cplusplus <= 201103L

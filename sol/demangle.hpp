// The MIT License (MIT) 

// Copyright (c) 2013-2016 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_DEMANGLE_HPP
#define SOL_DEMANGLE_HPP

#include <string>
#include <array>
#include <cstdlib>
#include <cctype>

#if defined(__GNUC__) || defined(__clang__)
#include <cxxabi.h>
#endif

namespace sol {
namespace detail {
#ifdef _MSC_VER
#ifndef SOL_NO_RTTI
inline std::string get_type_name(const std::type_info& id) {
    std::string realname = id.name();
    const static std::array<std::string, 2> removals = { { "struct ", "class " } };
    for (std::size_t r = 0; r < removals.size(); ++r) {
        auto found = realname.find(removals[r]);
        while (found != std::string::npos) {
            realname.erase(found, removals[r].size());
            found = realname.find(removals[r]);
        }
    }
    return realname;
}
#endif // No RTII
template <typename T>
inline std::string ctti_get_type_name() {
    std::string name = __FUNCSIG__;
    std::size_t start = name.find("get_type_name");
    if (start == std::string::npos)
        start = 0;
    else
        start += 13;
    if (start < name.size() - 1)
        start += 1;
    std::size_t end = name.find_last_of('>');
    if (end == std::string::npos)
        end = name.size();
    name = name.substr(start, end - start);
    if (name.find("struct", 0) == 0)
        name.replace(0, 6, "", 0);
    if (name.find("class", 0) == 0)
        name.replace(0, 5, "", 0);
    while (!name.empty() && std::isblank(name.front())) name.erase(name.begin(), ++name.begin());
    while (!name.empty() && std::isblank(name.back())) name.erase(--name.end(), name.end());
    return name;
}
#elif defined(__GNUC__) || defined(__clang__)
template <typename T>
inline std::string ctti_get_type_name() {
    std::string name = __PRETTY_FUNCTION__;
    std::size_t start = name.find_last_of('[');
    start = name.find_first_of('=', start);
    std::size_t end = name.find_last_of(']');
    if (end == std::string::npos)
        end = name.size();
    if (start == std::string::npos)
        start = 0;
    if (start < name.size() - 1)
        start += 1;
    name = name.substr(start, end - start);
    start = name.find(";");
    if (start != std::string::npos) {
        name.erase(start, name.length());
    }
    while (!name.empty() && std::isblank(name.front())) name.erase(name.begin(), ++name.begin());
    while (!name.empty() && std::isblank(name.back())) name.erase(--name.end(), name.end());
    return name;
}
#ifndef SOL_NO_RTTI
#if defined(__clang__)
inline std::string get_type_name(const std::type_info& id) {
    int status;
    char* unmangled = abi::__cxa_demangle(id.name(), 0, 0, &status);
    std::string realname = unmangled;
    std::free(unmangled);
    return realname;
}
#elif defined(__GNUC__)
inline std::string get_type_name(const std::type_info& id) {
    int status;
    char* unmangled = abi::__cxa_demangle(id.name(), 0, 0, &status);
    std::string realname = unmangled;
    std::free(unmangled);
    return realname;
}
#endif // g++ || clang++
#endif // No RTII
#else
#error Compiler not supported for demangling
#endif // compilers

template <typename T>
inline std::string demangle_once() {
#ifndef SOL_NO_RTTI
    std::string realname = get_type_name(typeid(T));
#else
    std::string realname = ctti_get_type_name<T>();
#endif // No Runtime Type Information
    return realname;
}

template <typename T>
inline std::string short_demangle_once() {
    std::string realname = ctti_get_type_name<T>();
    std::size_t idx = realname.find_last_of(":`'\"{}[]|-)(*^&!@#$%`~", std::string::npos, 23);
    if (idx != std::string::npos) {
        realname.erase(0, realname.length() < idx ? realname.length() : idx + 1);
    }
    return realname;
}

template <typename T>
inline std::string demangle() {
    static const std::string d = demangle_once<T>();
    return d;
}

template <typename T>
inline std::string short_demangle() {
    static const std::string d = short_demangle_once<T>();
    return d;
}
} // detail
} // sol

#endif // SOL_DEMANGLE_HPP

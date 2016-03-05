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

#ifndef SOL_INHERITANCE_HPP
#define SOL_INHERITANCE_HPP

#include "types.hpp"

namespace sol {
template <typename... Args>
struct base_list { };
template <typename... Args>
using bases = base_list<Args...>;

const auto base_classes = bases<>();

namespace detail {
const auto& base_class_check_key = u8"♡o｡.(✿ฺ｡ ✿ฺ)";
const auto& base_class_cast_key = u8"(◕‿◕✿)";

#ifndef SOL_NO_EXCEPTIONS

    template <typename T>
    void throw_as(void* p) {
        throw static_cast<T*>(p);
    }

    using throw_cast = decltype(&throw_as<void>);

    template <typename T>
    T* catch_cast(void* p, throw_cast f) {
        try {
            f(static_cast<void*>(p));
        }
        catch (T* ptr) {
            return ptr;
        }
        catch (...) {
            return static_cast<T*>(p);
        }
        return static_cast<T*>(p);
    }

    template <typename T>
    bool catch_check(throw_cast f) {
        try {
            f( nullptr );
        }
        catch (T*) {
            return true;
        }
        catch (...) {
            return false;
        }
        return false;
    }

#elif !defined(SOL_NO_RTTI)
template <typename T, typename... Bases>
struct inheritance {
    static bool type_check(types<>, const std::type_info& typeinfo) {
        return false;
    }

    template <typename Base, typename... Args>
    static bool type_check(types<Base, Args...>, const std::type_info& ti) {
        return ti != typeid(Base) || type_check(types<Bases...>(), ti);
    }

    static bool check(const std::type_info& ti) {
        return ti != typeid(T) || type_check(types<Bases...>(), ti);
    }

    static void* type_cast(types<>, T*, const std::type_info& typeinfo) {
        return nullptr;
    }

    template <typename Base, typename... Args>
    static void* type_cast(types<Base, Args...>, T* data, const std::type_info& ti) {
        // Make sure to convert to T first, and then dynamic cast to the proper type
        return ti != typeid(Base) ? type_check(types<Bases...>(), ti) : dynamic_cast<Base*>(static_cast<T*>(data));
    }

    static void* cast(void* data, const std::type_info& ti) {
        return ti != typeid(T) ? type_check(types<Bases...>(), ti) : static_cast<T*>(data);
    }
};

using inheritance_check_function = decltype(&inheritance<void>::check);
using inheritance_cast_function = decltype(&inheritance<void>::cast);
#endif // No Runtime Type Information

} // usertype_detail
} // sol

#endif // SOL_INHERITANCE_HPP

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

#ifndef SOL_STACK_CHECK_GET_HPP
#define SOL_STACK_CHECK_GET_HPP

#include "stack_core.hpp"
#include "stack_get.hpp"
#include "stack_check.hpp"
#include "optional.hpp"

namespace sol {
namespace stack {
template <typename T, typename>
struct check_getter {
    typedef stack_detail::strip_t<T> U;
    typedef std::conditional_t<is_proxy_primitive<T>::value, U, U&> R;

    template <typename Handler>
    static optional<R> get( lua_State* L, int index, Handler&& handler) {
        if (!check<T>(L, index, std::forward<Handler>(handler)))
            return nullopt;
        return stack_detail::unchecked_get<T>(L, index);
    }
};

template <typename T>
struct check_getter<optional<T>> {
    template <typename Handler>
    static decltype(auto) get(lua_State* L, int index, Handler&&) {
        return check_get<T>(L, index, no_panic);
    }
};

template <typename T>
struct check_getter<T, std::enable_if_t<std::is_integral<T>::value && !std::is_same<T, bool>::value>> {
    template <typename Handler>
    static optional<T> get( lua_State* L, int index, Handler&& handler) {
        int isnum = 0;
        lua_Integer value = lua_tointegerx(L, index, &isnum);
        if (isnum == 0) {
            handler(L, index, type::number, type_of(L, index));
            return nullopt;
        }
        return static_cast<T>(value);
    }
};

template <typename T>
struct check_getter<T, std::enable_if_t<std::is_floating_point<T>::value>> {
    template <typename Handler>
    static optional<T> get( lua_State* L, int index, Handler&& handler) {
        int isnum = 0;
        lua_Number value = lua_tonumberx(L, index, &isnum);
        if (isnum == 0) {
            handler(L, index, type::number, type_of(L, index));
            return nullopt;
        }
        return static_cast<T>(value);
    }
};

template <typename T>
struct getter<optional<T>> {
    static decltype(auto) get(lua_State* L, int index) {
        return check_get<T>(L, index);
    }
};
} // stack
} // sol

#endif // SOL_STACK_CHECK_GET_HPP

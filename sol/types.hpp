// The MIT License (MIT)

// Copyright (c) 2013 Danny Y., Rapptz

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

#ifndef SOL_TYPES_HPP
#define SOL_TYPES_HPP

#include <lua.hpp>
#include <string>
#include <type_traits>

namespace sol {
struct nil_t {};
const nil_t nil{};

enum class type : int {
    none = LUA_TNONE,
    nil = LUA_TNIL,
    string = LUA_TSTRING,
    number = LUA_TNUMBER,
    thread = LUA_TTHREAD,
    boolean = LUA_TBOOLEAN,
    function = LUA_TFUNCTION,
    userdata = LUA_TUSERDATA,
    lightuserdata = LUA_TLIGHTUSERDATA,
    table = LUA_TTABLE,
    poly = none | nil | string | number | thread | boolean | 
           function | userdata | lightuserdata | table
};

inline void type_error(lua_State* L, int expected, int actual) {
    luaL_error(L, "expected %s, received %s", lua_typename(L, expected), lua_typename(L, actual));
}

inline void type_assert(lua_State* L, int index, type expected) {
    int actual = lua_type(L, index);
    if(expected != type::poly && static_cast<int>(expected) != actual) {
        type_error(L, static_cast<int>(expected), actual);
    }
}

class table;
class function;
class object;

namespace detail {
template<typename T>
inline type arithmetic(std::true_type) {
    return type::number;
}

template<typename T>
inline type arithmetic(std::false_type) {
    return type::none;
}
} // detail

template<typename T>
inline type type_of() {
    return detail::arithmetic<T>(std::is_arithmetic<T>{});
}

template<>
inline type type_of<table>() {
    return type::table;
}

template<>
inline type type_of<function>() {
    return type::function;
}

template<>
inline type type_of<object>() {
    return type::poly;
}

template<>
inline type type_of<const char*>() {
    return type::string;
}

template<>
inline type type_of<std::string>() {
    return type::string;
}

template<>
inline type type_of<nil_t>() {
    return type::nil;
}

template<>
inline type type_of<bool>() {
    return type::boolean;
}
} // sol

#endif // SOL_TYPES_HPP
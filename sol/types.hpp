// The MIT License (MIT)

// Copyright (c) 2013-2015 Danny Y., Rapptz

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

#include "compatibility.hpp"
#include "traits.hpp"
#include <string>

namespace sol {
struct nil_t {};
const nil_t nil {};
inline bool operator==(nil_t, nil_t) { return true; }
inline bool operator!=(nil_t, nil_t) { return false; }

struct void_type : types<void> {}; // This is important because it allows myobject.call( Void, ... ) to work
const void_type Void {};

template<typename... T>
struct function_sig_t {};
using function_t = function_sig_t<>;

struct upvalue {
    void* value;
    upvalue(void* data) : value(data) {}
    operator void*() const { return value; }
};

struct light_userdata {
    void* value;
    light_userdata(void* data) : value(data) {}
    operator void*() const { return value; }
};

struct userdata {
    void* value;
    userdata(void* data) : value(data) {}
    operator void*() const { return value; }
};

enum class call_syntax {
    dot = 0,
    colon = 1
};

enum class call_error : int {
	ok = LUA_OK,
	runtime = LUA_ERRRUN,
	memory = LUA_ERRMEM,
	handler = LUA_ERRERR,
	gc = LUA_ERRGCMM
};

enum class type : int {
    none          = LUA_TNONE,
    nil           = LUA_TNIL,
    string        = LUA_TSTRING,
    number        = LUA_TNUMBER,
    thread        = LUA_TTHREAD,
    boolean       = LUA_TBOOLEAN,
    function      = LUA_TFUNCTION,
    userdata      = LUA_TUSERDATA,
    lightuserdata = LUA_TLIGHTUSERDATA,
    table         = LUA_TTABLE,
    poly          = none   | nil     | string   | number   | thread          |
                    table  | boolean | function | userdata | lightuserdata
};

inline int type_panic(lua_State* L, int index, type expected, type actual) {
    return luaL_error(L, "stack index %d, expected %s, received %s", index, lua_typename(L, static_cast<int>(expected)), lua_typename(L, static_cast<int>(actual)));
}

// Specify this function as the handler for lua::check if you know there's nothing wrong
inline int no_panic(lua_State*, int, type, type) {
    return 0;
}

inline void type_error(lua_State* L, int expected, int actual) {
    luaL_error(L, "expected %s, received %s", lua_typename(L, expected), lua_typename(L, actual));
}

inline void type_error(lua_State* L, type expected, type actual) {
    type_error(L, static_cast<int>(expected), static_cast<int>(actual));
}

inline void type_assert(lua_State* L, int index, type expected, type actual) {
    if (expected != type::poly && expected != actual) {
           type_panic(L, index, expected, actual);
    }
}

inline void type_assert(lua_State* L, int index, type expected) {
    int actual = lua_type(L, index);
    if(expected != type::poly && static_cast<int>(expected) != actual) {
           type_error(L, static_cast<int>(expected), actual);
    }
}

inline std::string type_name(lua_State*L, type t) {
    return lua_typename(L, static_cast<int>(t));
}

template<typename T>
class usertype;
class table;
class function;
class object;

template <typename T, typename = void>
struct lua_type_of : std::integral_constant<type, type::userdata> {};

template <>
struct lua_type_of<std::string> : std::integral_constant<type, type::string> {};

template <std::size_t N>
struct lua_type_of<char[N]> : std::integral_constant<type, type::string> {};

template <>
struct lua_type_of<const char*> : std::integral_constant<type, type::string> {};

template <>
struct lua_type_of<bool> : std::integral_constant<type, type::boolean> {};

template <>
struct lua_type_of<nil_t> : std::integral_constant<type, type::nil> {};

template <>
struct lua_type_of<table> : std::integral_constant<type, type::table> {};

template <>
struct lua_type_of<object> : std::integral_constant<type, type::poly> {};

template <>
struct lua_type_of<light_userdata> : std::integral_constant<type, type::lightuserdata> {};

template <>
struct lua_type_of<function> : std::integral_constant<type, type::function> {};

template <typename Signature>
struct lua_type_of<std::function<Signature>> : std::integral_constant<type, type::function>{};

template <typename T>
struct lua_type_of<T*> : std::integral_constant<type, type::userdata> {};

template <typename T>
struct lua_type_of<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> : std::integral_constant<type, type::number> {};

template<typename T>
inline type type_of() {
    return lua_type_of<Unqualified<T>>::value;
}

inline type type_of(lua_State* L, int index) {
    return static_cast<type>(lua_type(L, index));
}

// All enumerations are given and taken from lua
// as numbers as well
template <typename T>
struct lua_type_of<T, typename std::enable_if<std::is_enum<T>::value>::type> : std::integral_constant<type, type::number> {

};

} // sol

#endif // SOL_TYPES_HPP

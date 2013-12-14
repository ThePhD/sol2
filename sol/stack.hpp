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

#ifndef SOL_STACK_HPP
#define SOL_STACK_HPP

#include "reference.hpp"
#include "tuple.hpp"
#include "traits.hpp"
#include <utility>
#include <array>
#include <cstring>

namespace sol {
namespace stack {
namespace detail {
template<typename T>
inline T get_unsigned(lua_State* L, std::true_type, int index = -1) {
    return lua_tounsigned(L, index);
}

template<typename T>
inline T get_unsigned(lua_State* L, std::false_type, int index = -1) {
    return static_cast<T>(lua_tointeger(L, index));
}

template<typename T>
inline T get_arithmetic(lua_State* L, std::false_type, int index = -1) {
    // T is a floating point
    return static_cast<T>(lua_tonumber(L, index));
}

template<typename T>
inline T get_arithmetic(lua_State* L, std::true_type, int index = -1) {
    // T is an integral
    return get_unsigned<T>(L, std::is_unsigned<T>{}, index);
}

template<typename T>
inline T get_nil(lua_State* L, std::true_type, int index = -1) {
    if (lua_isnil(L, index) == 0)
        throw sol::sol_error("not nil");
    return nil_t{};
}

template<typename T>
inline T get_nil(lua_State* L, std::false_type, int index = -1) {
    // T is a class type
    return T(L, index);
}

template<typename T>
inline T get_helper(lua_State* L, std::true_type, int index = -1) {
    return get_nil<T>(L, std::is_same<nil_t, T>(), index);
}

template<typename T>
inline T get_helper(lua_State* L, std::false_type, int index = -1) {
    // T is a fundamental type
    return get_arithmetic<T>(L, std::is_integral<T>{}, index);
}

template<typename T>
inline void push_unsigned(lua_State* L, T x, std::true_type) {
    lua_pushunsigned(L, x);
}

template<typename T>
inline void push_unsigned(lua_State* L, T x, std::false_type) {
    lua_pushinteger(L, x);
}

template<typename T>
inline void push_arithmetic(lua_State* L, T x, std::true_type) {
    // T is an integral type
    push_unsigned(L, x, std::is_unsigned<T>{});
}

template<typename T>
inline void push_arithmetic(lua_State* L, T x, std::false_type) {
    // T is an floating point type
    lua_pushnumber(L, x);
}
} // detail

template<typename T>
inline T get(lua_State* L, int index = -1) {
    return detail::get_helper<T>(L, std::is_class<T>{}, index);
}

template<>
inline bool get<bool>(lua_State* L, int index) {
    return lua_toboolean(L, index) != 0;
}

template<>
inline std::string get<std::string>(lua_State* L, int index) {
    std::string::size_type len;
    auto str = lua_tolstring(L, index, &len);
    return { str, len };
}

template<>
inline const char* get<const char*>(lua_State* L, int index) {
    return lua_tostring(L, index);
}

template<typename T>
inline T pop(lua_State* L) {
    auto r = get<T>(L);
    lua_pop(L, 1);
    return r;
}

template<typename T>
inline EnableIf<std::is_arithmetic<T>> push(lua_State* L, T arithmetic) {
    detail::push_arithmetic(L, arithmetic, std::is_integral<T>{});
}

inline void push(lua_State*, reference& ref) {
    ref.push();
}

inline void push(lua_State* L, bool boolean) {
    lua_pushboolean(L, boolean);
}

inline void push(lua_State* L, const nil_t&) {
    lua_pushnil(L);
}

inline void push(lua_State* L, lua_CFunction func) {
    lua_pushcfunction(L, func);
}

inline void push(lua_State* L, lua_CFunction func, int n) {
    lua_pushcclosure(L, func, n);
}

inline void push(lua_State* L, void* userdata) {
    lua_pushlightuserdata(L, userdata);
}

template<size_t N>
inline void push(lua_State* L, const char (&str)[N]) {
    lua_pushlstring(L, str, N - 1);
}

inline void push(lua_State* L, const char* str) {
    lua_pushlstring(L, str, std::char_traits<char>::length(str));
}

inline void push(lua_State* L, const std::string& str) {
    lua_pushlstring(L, str.c_str(), str.size());
}

template<typename T>
inline void push_user(lua_State* L, T& userdata, const char* metatablekey) {
    T* pdatum = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
    T& datum = *pdatum;
    datum = userdata;
    if (metatablekey != nullptr) {
        lua_getfield(L, LUA_REGISTRYINDEX, metatablekey);
        lua_setmetatable(L, -2);
    }
}

template<typename T, size_t N>
inline void push(lua_State* L, const std::array<T, N>& data) {
    for (auto&& i : data) {
        push(L, i);
    }
}

template<typename T>
inline int push_user(lua_State* L, T& item) {
    typedef typename std::decay<T>::type TValue;
    const static std::size_t itemsize = sizeof(TValue);
    const static std::size_t voidsize = sizeof(void*);
    const static std::size_t voidsizem1 = voidsize - 1;
    const static std::size_t data_t_count = (sizeof(TValue) + voidsizem1) / voidsize;
    typedef std::array<void*, data_t_count> data_t;

    data_t data{{}};
    std::memcpy(std::addressof(data[0]), std::addressof(item), itemsize);
    push(L, data);
    return data_t_count;
}

namespace detail {
template<typename T, std::size_t... I>
inline void push(lua_State* L, indices<I...>, const T& tuplen) {
    using swallow = char[ 1 + sizeof...(I) ];
    swallow {'\0', (sol::stack::push(L, std::get<I>(tuplen)), '\0')... };
}

template<typename F, typename... Vs>
auto ltr_pop(lua_State*, F&& f, types<>, Vs&&... vs) -> decltype(f(std::forward<Vs>(vs)...)) {
    return f(std::forward<Vs>(vs)...);
}
template<typename F, typename Head, typename... Vs>
auto ltr_pop(lua_State* L, F&& f, types<Head>, Vs&&... vs) -> decltype(ltr_pop(L, std::forward<F>(f), types<>(), std::forward<Vs>(vs)..., pop<Head>(L))) {
    return ltr_pop(L, std::forward<F>(f), types<>(), std::forward<Vs>(vs)..., pop<Head>(L));
}
template<typename F, typename Head, typename... Tail, typename... Vs>
auto ltr_pop(lua_State* L, F&& f, types<Head, Tail...>, Vs&&... vs) -> decltype(f(std::forward<Vs>(vs)..., std::declval<Head>(), std::declval<Tail>()...)) {
    return ltr_pop(L, std::forward<F>(f), types<Tail...>(), std::forward<Vs>(vs)..., pop<Head>(L));
}
} // detail

template<typename... Args>
inline void push(lua_State* L, const std::tuple<Args...>& tuplen) {
    detail::push(L, build_indices<sizeof...(Args)>(), tuplen);
}

template<typename... Args, typename TFx>
inline auto pop_call(lua_State* L, TFx&& fx, types<Args...>) -> decltype(detail::ltr_pop(L, std::forward<TFx>(fx), types<Args...>())) {
    return detail::ltr_pop(L, std::forward<TFx>(fx), types<Args...>());
}

template<typename... Args>
void push_args(lua_State* L, Args&&... args) {
    using swallow = char[];
    void(swallow{'\0', (stack::push(L, std::forward<Args>(args)), '\0')... });
}
} // stack
} // sol

#endif // SOL_STACK_HPP
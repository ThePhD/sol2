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

#include "error.hpp"
#include "reference.hpp"
#include "tuple.hpp"
#include "traits.hpp"
#include <utility>
#include <array>
#include <cstring>
#include <functional>

namespace sol {
namespace stack {
namespace detail {
inline nil_t get(types<nil_t>, lua_State* L, int index = -1) {
    if (lua_isnil(L, index) == 0)
        throw sol::error("not nil");
    return nil_t{ };
}

inline lightuserdata_t get(types<lightuserdata_t>, lua_State* L, int index = -1) {
    return{ lua_touserdata(L, lua_upvalueindex(index)) };
}

inline userdata_t get(types<userdata_t>, lua_State* L, int index = -1) {
    return{ lua_touserdata(L, index) };
}

inline std::string get(types<std::string>, lua_State* L, int index = -1) {
    std::string::size_type len;
    auto str = lua_tolstring(L, index, &len);
    return{ str, len };
}

inline const char* get(types<const char*>, lua_State* L, int index = -1) {
    return lua_tostring(L, index);
}

inline type get(types<type>, lua_State* L, int index = -1) {
    return static_cast<type>(lua_type(L, index));
}

template <typename T, typename U = typename std::remove_reference<T>::type>
inline U get_sol_type(std::true_type, types<T>, lua_State* L, int index = -1) {
    return U(L, index);
}

template <typename T>
inline T& get_sol_type(std::false_type, types<T>, lua_State* L, int index = -1) {
    userdata_t udata = get(types<userdata_t>{}, L, index);
    T* obj = static_cast<T*>(udata.value);
    return *obj;
}

template <typename T, typename U = Unqualified<T>>
inline auto get(types<T> t, lua_State* L, int index = -1) -> decltype(get_sol_type(std::is_base_of<sol::reference, U>(), t, L, index)) {
    return get_sol_type(std::is_base_of<sol::reference, U>(), t, L, index);
}

template <typename Signature>
inline std::function<Signature> get(types<std::function<Signature>>, lua_State* L, int index = -1);

template<typename T>
inline T get_unsigned(std::true_type, lua_State* L, int index = -1) {
    return lua_tounsigned(L, index);
}

template<typename T>
inline T get_unsigned(std::false_type, lua_State* L, int index = -1) {
    return static_cast<T>(lua_tointeger(L, index));
}

template<typename T>
inline T get_arithmetic(std::false_type, lua_State* L, int index = -1) {
    // T is a floating point
    return static_cast<T>(lua_tonumber(L, index));
}

template<typename T>
inline T get_arithmetic(std::true_type, lua_State* L, int index = -1) {
    // T is an integral
    return get_unsigned<T>(std::is_unsigned<T>{}, L, index);
}

template<typename T>
inline T get_helper(std::true_type, lua_State* L, int index = -1) {
    // T is a fundamental type
    return get_arithmetic<T>(std::is_integral<T>{}, L, index);
}

template<>
inline bool get_helper<bool>(std::true_type, lua_State* L, int index) {
    return lua_toboolean(L, index) != 0;
}

template<typename T>
inline auto get_helper(std::false_type, lua_State* L, int index = -1) -> decltype(get(types<T>(), L, index)) {
    // T is a class
    return get(types<T>(), L, index);
}
} // detail

template<typename T, typename U = Unqualified<T>>
inline auto get(lua_State* L, int index = -1) -> decltype(detail::get_helper<U>(std::is_arithmetic<U>{}, L, index)) {
    return detail::get_helper<U>(std::is_arithmetic<U>{}, L, index);
}

template<typename T>
inline std::pair<T, int> get_user(lua_State* L, int index = 1) {
    const static std::size_t data_t_count = (sizeof(T)+(sizeof(void*)-1)) / sizeof(void*);
    typedef std::array<void*, data_t_count> data_t;
    data_t voiddata{ {} };
    for (std::size_t i = 0, d = 0; d < sizeof(T); ++i, d += sizeof(void*)) {
        voiddata[ i ] = stack::get<lightuserdata_t>(L, index++);
    }
    return std::pair<T, int>(*reinterpret_cast<T*>(static_cast<void*>(voiddata.data())), index);
}

template<typename T>
auto pop(lua_State* L) -> decltype(get<T>(L)) {
    auto&& r = get<T>(L);
    lua_pop(L, 1);
    return r;
}

template<typename T>
struct pusher;

template<typename T>
struct pusher {
    template<typename U = T, EnableIf<std::is_floating_point<U>> = 0>
    static void push(lua_State* L, const T& value) {
        lua_pushnumber(L, value);
    }

    template<typename U = T, EnableIf<std::is_integral<U>, std::is_signed<U>> = 0>
    static void push(lua_State* L, const T& value) {
        lua_pushinteger(L, value);
    }

    template<typename U = T, EnableIf<std::is_integral<U>, std::is_unsigned<U>> = 0>
    static void push(lua_State* L, const T& value) {
        lua_pushunsigned(L, value);
    }

    template<typename U = T, EnableIf<has_begin_end<U>, Not<has_key_value_pair<U>>> = 0>
    static void push(lua_State* L, const T& cont) {
        lua_createtable(L, cont.size(), 0);
        unsigned index = 1;
        for(auto&& i : cont) {
            // push the index
            pusher<unsigned>::push(L, index++);
            // push the value
            pusher<Unqualified<decltype(i)>>::push(L, i);
            // set the table
            lua_settable(L, -3);
        }
    }

    template<typename U = T, EnableIf<has_begin_end<U>, has_key_value_pair<U>> = 0>
    static void push(lua_State* L, const T& cont) {
        lua_createtable(L, cont.size(), 0);
        for(auto&& pair : cont) {
            pusher<Unqualified<decltype(pair.first)>>::push(L, pair.first);
            pusher<Unqualified<decltype(pair.second)>>::push(L, pair.second);
            lua_settable(L, -3);
        }
    }

    template<typename U = T, EnableIf<std::is_base_of<reference, U>> = 0>
    static void push(lua_State*, T& ref) {
        ref.push();
    }
};

template<>
struct pusher<bool> {
    static void push(lua_State* L, const bool& b) {
        lua_pushboolean(L, b);
    }
};

template<>
struct pusher<nil_t> {
    static void push(lua_State* L, const nil_t&) {
        lua_pushnil(L);
    }
};

template<>
struct pusher<lua_CFunction> {
    static void push(lua_State* L, lua_CFunction func) {
        lua_pushcfunction(L, func);
    }
};

template<>
struct pusher<void*> {
    static void push(lua_State* L, void* userdata) {
        lua_pushlightuserdata(L, userdata);
    }
};

template<>
struct pusher<const char*> {
    static void push(lua_State* L, const char* str) {
        lua_pushlstring(L, str, std::char_traits<char>::length(str));
    }
};

template<size_t N>
struct pusher<char[N]> {
    static void push(lua_State* L, const char (&str)[N]) {
        lua_pushlstring(L, str, N - 1);
    }
};

template<>
struct pusher<std::string> {
    static void push(lua_State* L, const std::string& str) {
        lua_pushlstring(L, str.c_str(), str.size());
    }
};

template<typename T>
inline void push(lua_State* L, T&& t) {
    pusher<Unqualified<T>>::push(L, std::forward<T>(t));
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
inline void push_as_upvalues(lua_State* L, const std::array<T, N>& data) {
    for(auto&& i : data) {
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
    push_as_upvalues(L, data);
    return data_t_count;
}

namespace detail {
template<typename T, std::size_t... I>
inline void push_tuple(lua_State* L, indices<I...>, T&& tuplen) {
    using swallow = char[1 + sizeof...(I)];
    swallow {'\0', (sol::stack::push(L, std::get<I>(tuplen)), '\0')... };
}

template<typename F, typename... Vs, typename... Args>
inline auto ltr_get(lua_State*, int, F&& f, types<Args...>, types<>, Vs&&... vs) -> decltype(f(std::forward<Vs>(vs)...)) {
    return f(std::forward<Vs>(vs)...);
}
template<typename F, typename Head, typename... Tail, typename... Vs, typename... Args>
inline auto ltr_get(lua_State* L, int index, F&& f, types<Args...> t, types<Head, Tail...>, Vs&&... vs) -> decltype(f(std::declval<Args>()...)) {
    return ltr_get(L, index + 1, std::forward<F>(f), t, types<Tail...>(), std::forward<Vs>(vs)..., stack::get<Head>(L, index));
}

template<typename F, typename... Vs, typename... Args>
inline auto ltr_pop(lua_State*, F&& f, types<Args...>, types<>, Vs&&... vs) -> decltype(f(std::forward<Vs>(vs)...)) {
    return f(std::forward<Vs>(vs)...);
}
template<typename F, typename Head, typename... Tail, typename... Vs, typename... Args>
inline auto ltr_pop(lua_State* L, F&& f, types<Args...> t, types<Head, Tail...>, Vs&&... vs) -> decltype(f(std::declval<Args>()...)) {
    return ltr_pop(L, std::forward<F>(f), t, types<Tail...>(), std::forward<Vs>(vs)..., stack::pop<Head>(L));
}

template<typename F, typename... Vs, typename... Args>
inline auto rtl_pop(lua_State*, F&& f, types<Args...>, types<>, Vs&&... vs) -> decltype(f(std::forward<Vs>(vs)...)) {
    return f(std::forward<Vs>(vs)...);
}
template<typename F, typename Head, typename... Tail, typename... Vs, typename... Args>
inline auto rtl_pop(lua_State* L, F&& f, types<Args...> t, types<Head, Tail...>, Vs&&... vs) -> decltype(f(std::declval<Args>()...)) {
    return rtl_pop(L, std::forward<F>(f), t, types<Tail...>(), stack::pop<Head>(L), std::forward<Vs>(vs)...);
}
} // detail

template<typename... Args>
struct pusher<std::tuple<Args...>> {
    static void push(lua_State* L, const std::tuple<Args...>& tuplen) {
        detail::push_tuple(L, build_indices<sizeof...(Args)>(), tuplen);
    }

    static void push(lua_State* L, std::tuple<Args...>&& tuplen) {
        detail::push_tuple(L, build_indices<sizeof...(Args)>(), std::move(tuplen));
    }
};

template<typename T>
inline void push_reverse(lua_State* L, T&& item) {
    push(L, std::forward<T>(item));
}

template<typename... Args>
inline void push_reverse(lua_State* L, const std::tuple<Args...>& tuplen) {
    detail::push_tuple(L, build_reverse_indices<sizeof...(Args)>(), tuplen);
}

template<typename... Args>
inline void push_reverse(lua_State* L, std::tuple<Args...>&& tuplen) {
    detail::push_tuple(L, build_reverse_indices<sizeof...(Args)>(), std::move(tuplen));
}

template<typename... Args, typename TFx>
inline auto get_call(lua_State* L, int index, TFx&& fx, types<Args...> t) -> decltype(detail::ltr_get(L, index, std::forward<TFx>(fx), t, t)) {
    return detail::ltr_get(L, index, std::forward<TFx>(fx), t, t);
}

template<typename... Args, typename TFx>
inline auto get_call(lua_State* L, TFx&& fx, types<Args...> t) -> decltype(detail::ltr_get(L, 1, std::forward<TFx>(fx), t, t)) {
    return detail::ltr_get(L, 1, std::forward<TFx>(fx), t, t);
}

template<typename... Args, typename TFx>
inline auto pop_call(lua_State* L, TFx&& fx, types<Args...> t) -> decltype(detail::ltr_pop(L, std::forward<TFx>(fx), t, t)) {
    return detail::ltr_pop(L, std::forward<TFx>(fx), t, t);
}

template<typename... Args, typename TFx>
inline auto pop_reverse_call(lua_State* L, TFx&& fx, types<Args...> t) -> decltype(detail::rtl_pop(L, std::forward<TFx>(fx), t, reversed<Args...>())) {
    return detail::rtl_pop(L, std::forward<TFx>(fx), t, reversed<Args...>());
}

inline void push_args(lua_State*) {

}

template<typename Arg, typename... Args>
inline void push_args(lua_State* L, Arg&& arg, Args&&... args) {
    using swallow = char[];
    stack::push(L, std::forward<Arg>(arg));
    void(swallow{'\0', (stack::push(L, std::forward<Args>(args)), '\0')... });
}

inline call_syntax get_call_syntax(lua_State* L, const std::string& meta) {
    if (get<type>(L, 1) == type::table) {
        if (luaL_newmetatable(L, meta.c_str()) == 0) {
            lua_settop(L, -2);
            return call_syntax::colon;
        }
    }
    return call_syntax::dot;
}

inline std::string dump_types(lua_State* L) {
    std::string visual;
    std::size_t size = lua_gettop(L) + 1;
    for (std::size_t i = 1; i < size; ++i) {
        if (i != 1)
            visual += " | ";
        visual += type_name(L, stack::get<type>(L, i));
    }
    return visual;
}

template <typename T>
struct get_return {
    typedef decltype(get<T>(nullptr)) type;
};
} // stack
} // sol

#endif // SOL_STACK_HPP

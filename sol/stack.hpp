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
#include "userdata_traits.hpp"
#include <utility>
#include <array>
#include <cstring>
#include <functional>

namespace sol {
namespace detail {
template<typename T>
inline T* get_ptr(T& val) {
    return std::addressof(val);
}

template<typename T>
inline T* get_ptr(std::reference_wrapper<T> val) {
    return std::addressof(val.get());
}

template<typename T>
inline T* get_ptr(T* val) {
    return val;
}

template<typename Decorated>
struct return_forward {
    typedef Unqualified<Decorated> T;

    T& operator()(T& value) const {
        return value;
    }

    T&& operator()(T&& value) const {
        return std::move(value);
    }

    T operator()(const T& value) const {
        return value;
    }

    // handle retarded libraries
    T operator()(const T&& value) const {
        return value;
    }
};
} // detail

namespace stack {
namespace detail {
template<typename T, typename Key, typename... Args>
inline void push_userdata(lua_State* L, Key&& metatablekey, Args&&... args) {
    T* pdatum = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
    std::allocator<T> alloc{};
    alloc.construct(pdatum, std::forward<Args>(args)...);
    luaL_getmetatable(L, std::addressof(metatablekey[0]));
    lua_setmetatable(L, -2);
}
} // detail
template<typename T, typename X = void>
struct getter;
template<typename T, typename X = void>
struct pusher;

template<typename T, typename>
struct getter {
    template<typename U = T, EnableIf<std::is_floating_point<U>> = 0>
    static U get(lua_State* L, int index = -1) {
        return static_cast<U>(lua_tonumber(L, index));
    }

    template<typename U = T, EnableIf<std::is_integral<U>, std::is_signed<U>> = 0>
    static U get(lua_State* L, int index = -1) {
        return static_cast<U>(lua_tounsigned(L, index));
    }

    template<typename U = T, EnableIf<std::is_integral<U>, std::is_unsigned<U>> = 0>
    static U get(lua_State* L, int index = -1) {
        return static_cast<T>(lua_tointeger(L, index));
    }

    template<typename U = T, EnableIf<std::is_base_of<reference, U>> = 0>
    static U get(lua_State* L, int index = -1) {
        return T(L, index);
    }

    template<typename U = T, EnableIf<Not<std::is_base_of<reference, U>>, Not<std::is_integral<U>>, Not<std::is_floating_point<U>>> = 0>
    static U& get(lua_State* L, int index = -1) {
        void* udata = lua_touserdata(L, index);
        T* obj = static_cast<T*>(udata);
        return *obj;
    }
};

template<typename T>
struct getter<T*> {
    static T* get(lua_State* L, int index = -1) {
        void* udata = lua_touserdata(L, index);
        T** obj = static_cast<T**>(udata);
        return *obj;
    }
};

template<>
struct getter<type> {
    static type get(lua_State *L, int index){
        return static_cast<type>(lua_type(L, index));
    }
};

template<>
struct getter<bool> {
    static bool get(lua_State* L, int index) {
        return lua_toboolean(L, index) != 0;
    }
};

template<>
struct getter<std::string> {
    static std::string get(lua_State* L, int index = -1) {
        std::string::size_type len;
        auto str = lua_tolstring(L, index, &len);
        return { str, len };
    }
};

template<>
struct getter<const char*> {
    static const char* get(lua_State* L, int index = -1) {
        return lua_tostring(L, index);
    }
};

template<>
struct getter<nil_t> {
    static nil_t get(lua_State* L, int index = -1) {
        if(lua_isnil(L, index) == 0) {
            throw sol::error("not nil");
        }
        return nil_t{ };
    }
};

template<>
struct getter<userdata_t> {
    static userdata_t get(lua_State* L, int index = -1) {
        return{ lua_touserdata(L, index) };
    }
};

template<>
struct getter<lightuserdata_t> {
    static lightuserdata_t get(lua_State* L, int index = 1) {
        return{ lua_touserdata(L, index) };
    }
};

template<>
struct getter<upvalue_t> {
    static upvalue_t get(lua_State* L, int index = 1) {
        return{ lua_touserdata(L, lua_upvalueindex(index)) };
    }
};

template<>
struct getter<void*> {
    static void* get(lua_State* L, int index = 1) {
        return lua_touserdata(L, index);
    }
};

template<typename T, typename>
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
            pusher<unsigned>{}.push(L, index++);
            // push the value
            pusher<Unqualified<decltype(i)>>{}.push(L, i);
            // set the table
            lua_settable(L, -3);
        }
    }

    template<typename U = T, EnableIf<has_begin_end<U>, has_key_value_pair<U>> = 0>
    static void push(lua_State* L, const T& cont) {
        lua_createtable(L, cont.size(), 0);
        for(auto&& pair : cont) {
            pusher<Unqualified<decltype(pair.first)>>{}.push(L, pair.first);
            pusher<Unqualified<decltype(pair.second)>>{}.push(L, pair.second);
            lua_settable(L, -3);
        }
    }

    template<typename U = T, EnableIf<std::is_base_of<reference, U>> = 0>
    static void push(lua_State*, T& ref) {
        ref.push();
    }

    template<typename U = Unqualified<T>, EnableIf<Not<has_begin_end<U>>, Not<std::is_base_of<reference, U>>, Not<std::is_integral<U>>, Not<std::is_floating_point<U>>> = 0>
    static void push(lua_State* L, T& t) {
        pusher<T*>{}.push(L, std::addressof(t));
    }

    template<typename U = Unqualified<T>, EnableIf<Not<has_begin_end<U>>, Not<std::is_base_of<reference, U>>, Not<std::is_integral<U>>, Not<std::is_floating_point<U>>> = 0>
    static void push(lua_State* L, T&& t) {
        detail::push_userdata<U>(L, userdata_traits<T>::metatable, std::move(t));
    }
};

template<typename T>
struct pusher<T*> {
    static void push(lua_State* L, T* obj) {
        detail::push_userdata<T*>(L, userdata_traits<T*>::metatable, obj);
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
    static void push(lua_State* L, lua_CFunction func, int n = 0) {
        lua_pushcclosure(L, func, n);
    }
};

template<>
struct pusher<void*> {
    static void push(lua_State* L, void* userdata) {
        lua_pushlightuserdata(L, userdata);
    }
};

template<>
struct pusher<upvalue_t> {
    static void push(lua_State* L, upvalue_t upvalue) {
        lua_pushlightuserdata(L, upvalue);
    }
};

template<>
struct pusher<lightuserdata_t> {
    static void push(lua_State* L, lightuserdata_t userdata) {
        lua_pushlightuserdata(L, userdata);
    }
};

template<>
struct pusher<userdata_t> {
    template<typename T, typename U = Unqualified<T>>
    static void push(lua_State* L, T&& data) {
        U* userdata = static_cast<U*>(lua_newuserdata(L, sizeof(U)));
        new(userdata)U(std::forward<T>(data));
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

template<typename T, typename... Args>
inline void push(lua_State* L, T&& t, Args&&... args) {
    pusher<Unqualified<T>>{}.push(L, std::forward<T>(t), std::forward<Args>(args)...);
}

// overload allows to use a pusher of a specific type, but pass in any kind of args
template<typename T, typename Arg, typename... Args>
inline void push(lua_State* L, Arg&& arg, Args&&... args) {
    pusher<Unqualified<T>>{}.push(L, std::forward<Arg>(arg), std::forward<Args>(args)...);
}

inline void push_args(lua_State*) {
    // do nothing
}

template<typename T, typename... Args>
inline void push_args(lua_State* L, T&& t, Args&&... args) {
    pusher<Unqualified<T>>{}.push(L, std::forward<T>(t));
    using swallow = char[];
    void(swallow{'\0', (pusher<Unqualified<Args>>{}.push(L, std::forward<Args>(args)), '\0')... });
}

template<typename T, typename U = Unqualified<T>>
inline auto get(lua_State* L, int index = -1) -> decltype(getter<U>{}.get(L, index)) {
    return getter<U>{}.get(L, index);
}

template<typename T>
auto pop(lua_State* L) -> decltype(get<T>(L)) {
    typedef decltype(get<T>(L)) ret_t;
    ret_t r = get<T>(L);
    lua_pop(L, 1);
    return r;
}

namespace detail {
template<typename T>
inline int push_as_upvalues(lua_State* L, T& item) {
    typedef typename std::decay<T>::type TValue;
    const static std::size_t itemsize = sizeof(TValue);
    const static std::size_t voidsize = sizeof(void*);
    const static std::size_t voidsizem1 = voidsize - 1;
    const static std::size_t data_t_count = (sizeof(TValue) + voidsizem1) / voidsize;
    typedef std::array<void*, data_t_count> data_t;

    data_t data{{}};
    std::memcpy(std::addressof(data[0]), std::addressof(item), itemsize);
    for(auto&& v : data) {
        push(L, upvalue_t(v));
    }
    return data_t_count;
}

template<typename T>
inline std::pair<T, int> get_as_upvalues(lua_State* L, int index = 1) {
    const static std::size_t data_t_count = (sizeof(T)+(sizeof(void*)-1)) / sizeof(void*);
    typedef std::array<void*, data_t_count> data_t;
    data_t voiddata{ {} };
    for(std::size_t i = 0, d = 0; d < sizeof(T); ++i, d += sizeof(void*)) {
        voiddata[i] = get<upvalue_t>(L, index++);
    }
    return std::pair<T, int>(*reinterpret_cast<T*>(static_cast<void*>(voiddata.data())), index);
}

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
inline auto ltr_get(lua_State* L, int index, F&& f, types<Args...>, types<Head, Tail...>, Vs&&... vs) -> decltype(f(std::declval<Args>()...)) {
    return ltr_get(L, index + 1, std::forward<F>(f), types<Args...>(), types<Tail...>(), std::forward<Vs>(vs)..., stack::get<Head>(L, index));
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

template<typename... Args, typename TFx, typename... Vs>
inline auto get_call(lua_State* L, int index, TFx&& fx, types<Args...> t, Vs&&... vs) -> decltype(detail::ltr_get(L, index, std::forward<TFx>(fx), t, t, std::forward<Vs>(vs)...)) {
    return detail::ltr_get(L, index, std::forward<TFx>(fx), t, t, std::forward<Vs>(vs)...);
}

template<typename TFx, typename... Args, typename... Vs>
inline auto get_call(lua_State* L, TFx&& fx, types<Args...> t, Vs&&... vs) -> decltype(get_call(L, 1, std::forward<TFx>(fx), t, std::forward<Vs>(vs)...)) {
    return get_call(L, 1, std::forward<TFx>(fx), t, std::forward<Vs>(vs)...);
}

template<typename... Args, typename TFx>
inline auto pop_call(lua_State* L, TFx&& fx, types<Args...> t) -> decltype(detail::ltr_pop(L, std::forward<TFx>(fx), t, t)) {
    return detail::ltr_pop(L, std::forward<TFx>(fx), t, t);
}

template<typename... Args, typename TFx>
inline auto pop_reverse_call(lua_State* L, TFx&& fx, types<Args...> t) -> decltype(detail::rtl_pop(L, std::forward<TFx>(fx), t, reversed<Args...>())) {
    return detail::rtl_pop(L, std::forward<TFx>(fx), t, reversed<Args...>());
}

inline call_syntax get_call_syntax(lua_State* L, const std::string& meta) {
    if(get<type>(L, 1) == type::table) {
        if(luaL_newmetatable(L, meta.c_str()) == 0) {
            lua_settop(L, -2);
            return call_syntax::colon;
        }
    }
    return call_syntax::dot;
}

template<typename T>
struct get_return {
    typedef decltype(get<T>(nullptr)) type;
};
} // stack
} // sol

#endif // SOL_STACK_HPP

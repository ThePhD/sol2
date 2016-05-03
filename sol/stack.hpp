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

#ifndef SOL_STACK_HPP
#define SOL_STACK_HPP

#include "stack_core.hpp"
#include "stack_reference.hpp"
#include "stack_check.hpp"
#include "stack_get.hpp"
#include "stack_check_get.hpp"
#include "stack_push.hpp"
#include "stack_pop.hpp"
#include "stack_field.hpp"
#include "stack_probe.hpp"
#include <cstring>
#include <array>

namespace sol {
namespace stack {
namespace stack_detail {
template <typename T>
struct is_this_state_raw : std::false_type {};
template <>
struct is_this_state_raw<this_state> : std::true_type {};
template <typename T>
using is_this_state = is_this_state_raw<meta::Unqualified<T>>;

template<typename T>
inline int push_as_upvalues(lua_State* L, T& item) {
    typedef std::decay_t<T> TValue;
    const static std::size_t itemsize = sizeof(TValue);
    const static std::size_t voidsize = sizeof(void*);
    const static std::size_t voidsizem1 = voidsize - 1;
    const static std::size_t data_t_count = (sizeof(TValue) + voidsizem1) / voidsize;
    typedef std::array<void*, data_t_count> data_t;

    data_t data{{}};
    std::memcpy(&data[0], std::addressof(item), itemsize);
    int pushcount = 0;
    for(auto&& v : data) {
        pushcount += push(L, light_userdata_value(v));
    }
    return pushcount;
}

template<typename T>
inline std::pair<T, int> get_as_upvalues(lua_State* L, int index = 1) {
    const static std::size_t data_t_count = (sizeof(T)+(sizeof(void*)-1)) / sizeof(void*);
    typedef std::array<void*, data_t_count> data_t;
    data_t voiddata{ {} };
    for(std::size_t i = 0, d = 0; d < sizeof(T); ++i, d += sizeof(void*)) {
        voiddata[i] = get<light_userdata_value>(L, up_value_index(index++));
    }
    return std::pair<T, int>(*reinterpret_cast<T*>(static_cast<void*>(voiddata.data())), index);
}

template <bool checkargs = default_check_arguments, std::size_t... I, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline decltype(auto) call(types<R>, types<Args...> ta, std::index_sequence<I...> tai, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    typedef meta::index_in_pack<this_state, Args...> state_index;
    typedef meta::index_in_pack<variadic_args, Args...> va_pack_index;
    check_types<checkargs>{}.check(ta, tai, L, start, type_panic);
    return fx(std::forward<FxArgs>(args)..., stack_detail::unchecked_get<Args>(L, start + I - static_cast<int>(state_index::value < I) - static_cast<int>(va_pack_index::value < I))...);
}

template <bool checkargs = default_check_arguments, std::size_t... I, typename... Args, typename Fx, typename... FxArgs>
inline void call(types<void>, types<Args...> ta, std::index_sequence<I...> tai, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    typedef meta::index_in_pack<this_state, Args...> state_index;
    typedef meta::index_in_pack<variadic_args, Args...> va_pack_index;
    check_types<checkargs>{}.check(ta, tai, L, start, type_panic);
    fx(std::forward<FxArgs>(args)..., stack_detail::unchecked_get<Args>(L, start + I - static_cast<int>(state_index::value < I) - static_cast<int>(va_pack_index::value < I))...);
}
} // stack_detail

inline void remove( lua_State* L, int index, int count ) {
    if ( count < 1 )
        return;
    int top = lua_gettop(L);
    if ( index == -1 || top == index ) {
        // Slice them right off the top
        lua_pop( L, static_cast<int>(count) );
        return;
    }

    // Remove each item one at a time using stack operations
    // Probably slower, maybe, haven't benchmarked,
    // but necessary
    if ( index < 0 ) {
        index = lua_gettop(L) + (index + 1);
    }
    int last = index + count;
    for ( int i = index; i < last; ++i ) {
        lua_remove(L, i);
    }
}

template <bool check_args = stack_detail::default_check_arguments, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline decltype(auto) call(types<R> tr, types<Args...> ta, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    typedef std::make_index_sequence<sizeof...(Args)> args_indices;
    return stack_detail::call<check_args>(tr, ta, args_indices(), L, start, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline decltype(auto) call(types<R> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    return call<check_args>(tr, ta, L, 1, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline void call(types<void> tr, types<Args...> ta, lua_State* L, int start, Fx&& fx, FxArgs&&... args) {
    typedef std::make_index_sequence<sizeof...(Args)> args_indices;
    stack_detail::call<check_args>(tr, ta, args_indices(), L, start, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline void call(types<void> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    call<check_args>(tr, ta, L, 1, std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename R, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<!std::is_void<R>::value>>
inline decltype(auto) call_from_top(types<R> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    return call<check_args>(tr, ta, L, static_cast<int>(lua_gettop(L) - sizeof...(Args)), std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template <bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline void call_from_top(types<void> tr, types<Args...> ta, lua_State* L, Fx&& fx, FxArgs&&... args) {
    call<check_args>(tr, ta, L, static_cast<int>(lua_gettop(L) - sizeof...(Args)), std::forward<Fx>(fx), std::forward<FxArgs>(args)...);
}

template<int additionalpop = 0, bool check_args = stack_detail::default_check_arguments, typename... Args, typename Fx, typename... FxArgs>
inline int call_into_lua(types<void> tr, types<Args...> ta, lua_State* L, int start, Fx&& fx, FxArgs&&... fxargs) {
    call<check_args>(tr, ta, L, start, std::forward<Fx>(fx), std::forward<FxArgs>(fxargs)...);
    int nargs = static_cast<int>(sizeof...(Args)) + additionalpop - meta::count_if_pack<stack_detail::is_this_state, Args...>::value;
    lua_pop(L, nargs);
    return 0;
}

template<int additionalpop = 0, bool check_args = stack_detail::default_check_arguments, typename Ret0, typename... Ret, typename... Args, typename Fx, typename... FxArgs, typename = std::enable_if_t<meta::Not<std::is_void<Ret0>>::value>>
inline int call_into_lua(types<Ret0, Ret...>, types<Args...> ta, lua_State* L, int start, Fx&& fx, FxArgs&&... fxargs) {
    decltype(auto) r = call<check_args>(types<meta::return_type_t<Ret0, Ret...>>(), ta, L, start, std::forward<Fx>(fx), std::forward<FxArgs>(fxargs)...);
    int nargs = static_cast<int>(sizeof...(Args)) + additionalpop - meta::count_if_pack<stack_detail::is_this_state, Args...>::value;
    lua_pop(L, nargs);
    return push_reference(L, std::forward<decltype(r)>(r));
}

inline call_syntax get_call_syntax(lua_State* L, const std::string& key, int index = -2) {
    luaL_getmetatable(L, key.c_str());
    if (lua_compare(L, -1, index, LUA_OPEQ) == 1) {
        lua_pop(L, 1);
        return call_syntax::colon;
    }
    lua_pop(L, 1);
    return call_syntax::dot;
}

inline void luajit_exception_handler(lua_State* L, int(*handler)(lua_State*, lua_CFunction) = detail::c_trampoline) {
#ifdef SOL_LUAJIT
    lua_pushlightuserdata(L, (void*)handler);
    luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
    lua_pop(L, 1);
#else
    (void)L;
    (void)handler;
#endif
}

inline void luajit_exception_off(lua_State* L) {
#ifdef SOL_LUAJIT
    luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_OFF);
#else
    (void)L;
#endif
}
} // stack
} // sol

#endif // SOL_STACK_HPP

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

#ifndef SOL_FUNCTION_TYPES_TEMPLATED_HPP
#define SOL_FUNCTION_TYPES_TEMPLATED_HPP

#include "stack.hpp"

namespace sol {
namespace function_detail {
    template <typename F, F fx>
    inline int call_wrapper_variable(std::false_type, lua_State* L) {
        typedef meta::bind_traits<meta::Unqualified<F>> traits_type;
        typedef typename traits_type::args_type args_type;
        typedef meta::tuple_types<typename traits_type::return_type> return_type;
        return stack::call_into_lua(return_type(), args_type(), L, 1, fx);
    }

    template <typename R, typename V, V, typename T>
    inline int call_set_assignable(std::false_type, T&&, lua_State* L) {
        lua_pop(L, 2);
        return luaL_error(L, "cannot write to this type: copy assignment/constructor not available");
    }

    template <typename R, typename V, V variable, typename T>
    inline int call_set_assignable(std::true_type, lua_State* L, T&& mem) {
        (mem.*variable) = stack::get<R>(L, 2);
        lua_pop(L, 2);
        return 0;
    }

    template <typename R, typename V, V, typename T>
    inline int call_set_variable(std::false_type, lua_State* L, T&&) {
        lua_pop(L, 2);
        return luaL_error(L, "cannot write to a const variable");
    }

    template <typename R, typename V, V variable, typename T>
    inline int call_set_variable(std::true_type, lua_State* L, T&& mem) {
        return call_set_assignable<R, V, variable>(std::is_assignable<std::add_lvalue_reference_t<R>, R>(), L, std::forward<T>(mem));
    }

    template <typename V, V variable>
    inline int call_wrapper_variable(std::true_type, lua_State* L) {
        typedef meta::bind_traits<meta::Unqualified<V>> traits_type;
        typedef typename traits_type::object_type T;
        typedef typename traits_type::return_type R;
        auto& mem = stack::get<T>(L, 1);
        switch (lua_gettop(L)) {
        case 1: {
            decltype(auto) r = (mem.*variable);
            lua_pop(L, 1);
            stack::push_reference(L, std::forward<decltype(r)>(r));
            return 1; }
        case 2:
            return call_set_variable<R, V, variable>(meta::Not<std::is_const<R>>(), L, mem);
        default:
            return luaL_error(L, "incorrect number of arguments to member variable function call");
        }
    }

    template <typename F, F fx>
    inline int call_wrapper_function(std::false_type, lua_State* L) {
        return call_wrapper_variable<F, fx>(std::is_member_object_pointer<F>(), L);
    }

    template <typename F, F fx>
    inline int call_wrapper_function(std::true_type, lua_State* L) {
        typedef meta::bind_traits<meta::Unqualified<F>> traits_type;
        typedef typename traits_type::object_type T;
        typedef typename traits_type::args_type args_type;
        typedef typename traits_type::return_type return_type;
        typedef meta::tuple_types<return_type> return_type_list;
        auto mfx = [&](auto&&... args) -> typename traits_type::return_type {
            auto& member = stack::get<T>(L, 1);
            return (member.*fx)(std::forward<decltype(args)>(args)...);
        };
        int n = stack::call_into_lua<1>(return_type_list(), args_type(), L, 2, mfx);
        return n;
    }

    template <typename F, F fx>
    inline int call_wrapper_entry(lua_State* L) {
        return call_wrapper_function<F, fx>(std::is_member_function_pointer<meta::Unqualified<F>>(), L);
    }
} // function_detail

template <typename F, F fx>
inline int c_call(lua_State* L) {
    return detail::c_trampoline(L, function_detail::call_wrapper_entry<F, fx>);
}
} // sol

#endif // SOL_FUNCTION_TYPES_TEMPLATED_HPP

// The MIT License (MIT)

// Copyright (c) 2013-2016 Rapptz and contributors

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

#ifndef SOL_FUNCTION_TYPES_STATIC_HPP
#define SOL_FUNCTION_TYPES_STATIC_HPP

#include "stack.hpp"

namespace sol {
template<typename Function>
struct static_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef function_traits<function_type> traits_type;

    template<typename... Args>
    static int typed_call(types<void> tr, types<Args...> ta, function_type* fx, lua_State* L) {
        stack::call(L, 0, tr, ta, fx);
        int nargs = static_cast<int>(sizeof...(Args));
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Ret, typename... Args>
    static int typed_call(types<Ret...>, types<Args...> ta, function_type* fx, lua_State* L) {
        typedef return_type_t<Ret...> return_type;
        decltype(auto) r = stack::call(L, 0, types<return_type>(), ta, fx);
        int nargs = static_cast<int>(sizeof...(Args));
        lua_pop(L, nargs);
        return stack::push(L, std::forward<decltype(r)>(r));
    }

    static int call(lua_State* L) {
        auto udata = stack::detail::get_as_upvalues<function_type*>(L);
        function_type* fx = udata.first;
        int r = typed_call(tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), fx, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename Function>
struct static_member_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef function_traits<function_type> traits_type;

    template<typename... Args>
    static int typed_call(types<void> tr, types<Args...> ta, T& item, function_type& ifx, lua_State* L) {
        auto fx = [&item, &ifx](Args&&... args) -> void { (item.*ifx)(std::forward<Args>(args)...); };
        stack::call(L, 0, tr, ta, fx);
        int nargs = static_cast<int>(sizeof...(Args));
        lua_pop(L, nargs);
        return 0;
    }

    template<typename... Ret, typename... Args>
    static int typed_call(types<Ret...> tr, types<Args...> ta, T& item, function_type& ifx, lua_State* L) {
        auto fx = [&item, &ifx](Args&&... args) -> return_type { return (item.*ifx)(std::forward<Args>(args)...); };
        decltype(auto) r = stack::call(L, 0, tr, ta, fx);
        int nargs = static_cast<int>(sizeof...(Args));
        lua_pop(L, nargs);
        return stack::push(L, std::forward<decltype(r)>(r));
    }

    static int call(lua_State* L) {
        auto memberdata = stack::detail::get_as_upvalues<function_type>(L, 1);
        auto objdata = stack::detail::get_as_upvalues<T*>(L, memberdata.second);
        function_type& memfx = memberdata.first;
        T& obj = *objdata.first;
        int r = typed_call(tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), obj, memfx, L);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};
} // sol

#endif // SOL_FUNCTION_TYPES_STATIC_HPP

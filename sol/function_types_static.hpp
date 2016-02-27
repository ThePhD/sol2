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

#ifndef SOL_FUNCTION_TYPES_STATIC_HPP
#define SOL_FUNCTION_TYPES_STATIC_HPP

#include "stack.hpp"

namespace sol {
namespace function_detail {
template<typename Function>
struct static_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::function_traits<function_type> traits_type;

    static int call(lua_State* L) {
        auto udata = stack::stack_detail::get_as_upvalues<function_type*>(L);
        function_type* fx = udata.first;
        int r = stack::call_into_lua(meta::tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), fx, L, 1);
        return r;
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename Function>
struct static_member_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::function_traits<function_type> traits_type;

    static int call(lua_State* L) {
        auto memberdata = stack::stack_detail::get_as_upvalues<function_type>(L, 1);
        auto objdata = stack::stack_detail::get_as_upvalues<T*>(L, memberdata.second);
        function_type& memfx = memberdata.first;
        T& item = *objdata.first;
        auto fx = [&item, &memfx](auto&&... args) -> typename traits_type::return_type { return (item.*memfx)(std::forward<decltype(args)>(args)...); };
        return stack::call_into_lua(meta::tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), fx, L, 1);
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};
} // function_detail
} // sol

#endif // SOL_FUNCTION_TYPES_STATIC_HPP

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

#ifndef SOL_FUNCTION_TYPES_BASIC_HPP
#define SOL_FUNCTION_TYPES_BASIC_HPP

#include "stack.hpp"

namespace sol {
namespace function_detail {
template<typename Function>
struct upvalue_free_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::bind_traits<function_type> traits_type;

    static int real_call(lua_State* L) {
        auto udata = stack::stack_detail::get_as_upvalues<function_type*>(L);
        function_type* fx = udata.first;
        int r = stack::call_into_lua(meta::tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), L, 1, fx);
        return r;
    }

    static int call (lua_State* L) {
        return detail::static_trampoline<(&real_call)>(L);
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename Function>
struct upvalue_member_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::bind_traits<function_type> traits_type;

    static int real_call(lua_State* L) {
        // Layout:
        // idx 1...n: verbatim data of member function pointer
        // idx n + 1: is the object's void pointer
        // We don't need to store the size, because the other side is templated
        // with the same member function pointer type
        auto memberdata = stack::stack_detail::get_as_upvalues<function_type>(L, 1);
        auto objdata = stack::stack_detail::get_as_upvalues<T*>(L, memberdata.second);
        function_type& memfx = memberdata.first;
        auto& item = *objdata.first;
        auto fx = [&item, &memfx](auto&&... args) -> typename traits_type::return_type { 
            return (item.*memfx)(std::forward<decltype(args)>(args)...);
        };
        return stack::call_into_lua(meta::tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), L, 1, fx);
    }

    static int call (lua_State* L) {
        return detail::static_trampoline<(&real_call)>(L);
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template <int N, typename R, typename M, typename V>
int set_assignable(std::false_type, lua_State* L, M&, V&) {
    lua_pop(L, N);
    return luaL_error(L, "sol: cannot write to this type: copy assignment/constructor not available");
}

template <int N, typename R, typename M, typename V>
int set_assignable(std::true_type, lua_State* L, M& mem, V& var) {
    (mem.*var) = stack::get<R>(L, N);
    lua_pop(L, N);
    return 0;
}

template <int N, typename R, typename M, typename V>
int set_variable(std::true_type, lua_State* L, M& mem, V& var) {
    return set_assignable<N, R>(std::is_assignable<std::add_lvalue_reference_t<R>, R>(), L, mem, var);
}

template <int N, typename R, typename M, typename V>
int set_variable(std::false_type, lua_State* L, M&, V&) {
    lua_pop(L, N);
    return luaL_error(L, "sol: cannot write to a const variable");
}

template<typename T, typename Function>
struct upvalue_member_variable {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::bind_traits<function_type> traits_type;

    static int real_call(lua_State* L) {
        // Layout:
        // idx 1...n: verbatim data of member variable pointer
        // idx n + 1: is the object's void pointer
        // We don't need to store the size, because the other side is templated
        // with the same member function pointer type
        auto memberdata = stack::stack_detail::get_as_upvalues<function_type>(L, 1);
        auto objdata = stack::stack_detail::get_as_upvalues<T*>(L, memberdata.second);
        auto& mem = *objdata.first;
        function_type& var = memberdata.first;
        switch (lua_gettop(L)) {
        case 0:
            stack::push(L, (mem.*var));
            return 1;
        case 1:
            set_variable<1, typename traits_type::return_type>(meta::Not<std::is_const<typename traits_type::return_type>>(), L, mem, var);
            return 0;
        default:
            return luaL_error(L, "sol: incorrect number of arguments to member variable function");
        }
    }

    static int call (lua_State* L) {
        return detail::static_trampoline<(&real_call)>(L);
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename Function>
struct upvalue_this_member_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::bind_traits<function_type> traits_type;

    static int real_call(lua_State* L) {
        // Layout:
        // idx 1...n: verbatim data of member variable pointer
        auto memberdata = stack::stack_detail::get_as_upvalues<function_type>(L, 1);
        function_type& memfx = memberdata.first;
        auto fx = [&L, &memfx](auto&&... args) -> typename traits_type::return_type { 
            auto& item = stack::get<T>(L, 1);
            return (item.*memfx)(std::forward<decltype(args)>(args)...);
        };
        int n = stack::call_into_lua<1>(meta::tuple_types<typename traits_type::return_type>(), typename traits_type::args_type(), L, 2, fx);
        return n;
    }

    static int call (lua_State* L) {
        return detail::static_trampoline<(&real_call)>(L);
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};

template<typename T, typename Function>
struct upvalue_this_member_variable {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::bind_traits<function_type> traits_type;

    static int real_call(lua_State* L) {
        // Layout:
        // idx 1...n: verbatim data of member variable pointer
        auto memberdata = stack::stack_detail::get_as_upvalues<function_type>(L, 1);
        auto& mem = stack::get<T>(L, 1);
        function_type& var = memberdata.first;
        switch (lua_gettop(L)) {
        case 1:
            lua_pop(L, 1);
            stack::push(L, (mem.*var));
            return 1;
        case 2:
            set_variable<2, typename traits_type::return_type>(meta::Not<std::is_const<typename traits_type::return_type>>(), L, mem, var);
            return 0;
        default:
            return luaL_error(L, "sol: incorrect number of arguments to member variable function");
        }
    }

    static int call (lua_State* L) {
        return detail::static_trampoline<(&real_call)>(L);
    }

    int operator()(lua_State* L) {
        return call(L);
    }
};
} // function_detail
} // sol

#endif // SOL_FUNCTION_TYPES_BASIC_HPP

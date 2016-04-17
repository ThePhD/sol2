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

#ifndef SOL_FUNCTION_TYPES_MEMBER_HPP
#define SOL_FUNCTION_TYPES_MEMBER_HPP

#include "function_types_core.hpp"

namespace sol {
namespace function_detail {
template<typename Func>
struct free_function : public base_function {
    typedef meta::Unwrapped<meta::Unqualified<Func>> Function;
    typedef meta::function_return_t<Function> return_type;
    typedef meta::function_args_t<Function> args_types;
    Function fx;

    template<typename... Args>
    free_function(Args&&... args): fx(std::forward<Args>(args)...) {}

    int call(lua_State* L) {
        return stack::call_into_lua(meta::tuple_types<return_type>(), args_types(), L, 1, fx);
    }

    virtual int operator()(lua_State* L) override {
        auto f = [&](lua_State* L) -> int { return this->call(L);};
        return detail::trampoline(L, f);
    }
};

template<typename Func>
struct functor_function : public base_function {
    typedef meta::Unwrapped<meta::Unqualified<Func>> Function;
    typedef decltype(&Function::operator()) function_type;
    typedef meta::function_return_t<function_type> return_type;
    typedef meta::function_args_t<function_type> args_types;
    Function fx;

    template<typename... Args>
    functor_function(Args&&... args): fx(std::forward<Args>(args)...) {}

    int call(lua_State* L) {
        return stack::call_into_lua(meta::tuple_types<return_type>(), args_types(), L, 1, fx);
    }

    virtual int operator()(lua_State* L) override {
        auto f = [&](lua_State* L) -> int { return this->call(L);};
        return detail::trampoline(L, f);
    }
};

template<typename T, typename Function>
struct member_function : public base_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::function_return_t<function_type> return_type;
    typedef meta::function_args_t<function_type> args_types;
    struct functor {
        function_type invocation;
        T member;
        
        template<typename F, typename... Args>
        functor(F&& f, Args&&... args): invocation(std::forward<F>(f)), member(std::forward<Args>(args)...) {}

        template<typename... Args>
        return_type operator()(Args&&... args) {
            auto& mem = detail::unwrap(detail::deref(member));
            return (mem.*invocation)(std::forward<Args>(args)...);
        }
    } fx;

    template<typename F, typename... Args>
    member_function(F&& f, Args&&... args) : fx(std::forward<F>(f), std::forward<Args>(args)...) {}

    int call(lua_State* L) {
        return stack::call_into_lua(meta::tuple_types<return_type>(), args_types(), L, 1, fx);
    }

    virtual int operator()(lua_State* L) override {
        auto f = [&](lua_State* L) -> int { return this->call(L);};
        return detail::trampoline(L, f);
    }
};

template<typename T, typename Function>
struct member_variable : public base_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef typename meta::bind_traits<function_type>::return_type return_type;
    typedef typename meta::bind_traits<function_type>::args_type args_types;
    function_type var;
    T member;
    typedef std::add_lvalue_reference_t<meta::Unwrapped<std::remove_reference_t<decltype(detail::deref(member))>>> M;

    template<typename V, typename... Args>
    member_variable(V&& v, Args&&... args): var(std::forward<V>(v)), member(std::forward<Args>(args)...) {}

    int set_assignable(std::false_type, lua_State* L, M) {
        lua_pop(L, 1);
        return luaL_error(L, "sol: cannot write to this type: copy assignment/constructor not available");
    }

    int set_assignable(std::true_type, lua_State* L, M mem) {
        (mem.*var) = stack::get<return_type>(L, 1);
        lua_pop(L, 1);
        return 0;
    }

    int set_variable(std::true_type, lua_State* L, M mem) {
        return set_assignable(std::is_assignable<std::add_lvalue_reference_t<return_type>, return_type>(), L, mem);
    }

    int set_variable(std::false_type, lua_State* L, M) {
        lua_pop(L, 1);
        return luaL_error(L, "sol: cannot write to a const variable");
    }

    int call(lua_State* L) {
        M mem = detail::unwrap(detail::deref(member));
        switch (lua_gettop(L)) {
        case 0:
            stack::push(L, (mem.*var));
            return 1;
        case 1:
            return set_variable(meta::Not<std::is_const<return_type>>(), L, mem);
        default:
            return luaL_error(L, "sol: incorrect number of arguments to member variable function");
        }
    }

    virtual int operator()(lua_State* L) override {
        auto f = [&](lua_State* L) -> int { return this->call(L);};
        return detail::trampoline(L, f);
    }
};
} // function_detail
} // sol

#endif // SOL_FUNCTION_TYPES_MEMBER_HPP

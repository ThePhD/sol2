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
template<typename Function>
struct functor_function : public base_function {
    typedef decltype(&Function::operator()) function_type;
    typedef meta::function_return_t<function_type> return_type;
    typedef meta::function_args_t<function_type> args_type;
    Function fx;

    template<typename... Args>
    functor_function(Args&&... args): fx(std::forward<Args>(args)...) {}

    int call(lua_State* L) {
        return stack::call_into_lua(meta::tuple_types<return_type>(), args_types(), fx, L, 1);
    }

    virtual int operator()(lua_State* L) override {
        auto f = [&](lua_State* L) -> int { return this->call(L);};
        return detail::trampoline(L, f);
    }
};

template<typename Function, typename T>
struct member_function : public base_function {
    typedef std::remove_pointer_t<std::decay_t<Function>> function_type;
    typedef meta::function_return_t<function_type> return_type;
    typedef meta::function_args_t<function_type> args_types;
    struct functor {
        T member;
        function_type invocation;

        template<typename Tm, typename... Args>
        functor(Tm&& m, Args&&... args): member(std::forward<Tm>(m)), invocation(std::forward<Args>(args)...) {}

        template<typename... Args>
        return_type operator()(Args&&... args) {
            auto& mem = detail::unwrap(detail::deref(member));
            return (mem.*invocation)(std::forward<Args>(args)...);
        }
    } fx;

    template<typename Tm, typename... Args>
    member_function(Tm&& m, Args&&... args): fx(std::forward<Tm>(m), std::forward<Args>(args)...) {}

    int call(lua_State* L) {
        return stack::call_into_lua(meta::tuple_types<return_type>(), args_types(), fx, L, 1);
    }

    virtual int operator()(lua_State* L) override {
        auto f = [&](lua_State* L) -> int { return this->call(L);};
        return detail::trampoline(L, f);
    }
};
} // function_detail
} // sol

#endif // SOL_FUNCTION_TYPES_MEMBER_HPP

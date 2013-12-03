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

#ifndef SOL_LUA_FUNC_HPP
#define SOL_LUA_FUNC_HPP

#include "functional.hpp"
#include "stack.hpp"

namespace sol {
namespace detail {

struct lua_func {
    virtual int operator()(lua_State*) {
        throw sol_error("Failure to call specialized wrapped C++ function from lua");
    }

    virtual ~lua_func() {};
};

template<typename TFx>
struct lambda_lua_func : public lua_func {
    typedef decltype(&TFx::operator()) fx_t;
    typedef function_traits<fx_t> fx_traits;
    TFx fx;

    template<typename... FxArgs>
    lambda_lua_func(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... TRn, typename... Args>
    int operator()(types<TRn...>, types<Args...> t, lua_State* L) {
        auto r = stack::pop_call(L, fx, t);
        stack::push(L, r);
        return sizeof...(TRn);
    }
};

template<typename TFx, typename T = TFx, bool is_member_pointer = std::is_member_function_pointer<TFx>::value>
struct explicit_lua_func : public lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    TFx fx;

    template<typename... FxArgs>
    explicit_lua_func(FxArgs&&... fxargs): fx(std::forward<FxArgs>(fxargs)...) {}

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... TRn, typename... Args>
    int operator()(types<TRn...>, types<Args...> t, lua_State* L) {
        auto r = stack::pop_call(L, fx, t);
        stack::push(L, r);
        return sizeof...(TRn);
    }
};

template<typename TFx, typename T>
struct explicit_lua_func<TFx, T, true> : public lua_func {
    typedef typename std::remove_pointer<typename std::decay<TFx>::type>::type fx_t;
    typedef function_traits<fx_t> fx_traits;
    struct lambda {
        T* member;
        TFx invocation;

        template<typename... FxArgs>
        lambda(T* m, FxArgs&&... fxargs): member(m), invocation(std::forward<FxArgs>(fxargs)...) {}

        template<typename... Args>
        typename fx_traits::return_type operator()(Args&&... args) {
           return ((*member).*invocation)(std::forward<Args>(args)...);
        }
    } fx;

    template<typename... FxArgs>
    explicit_lua_func(T* m, FxArgs&&... fxargs): fx(m, std::forward<FxArgs>(fxargs)...) {}

    template<typename... FxArgs>
    explicit_lua_func(T& m, FxArgs&&... fxargs): fx(std::addressof(m), std::forward<FxArgs>(fxargs)...) {}

    virtual int operator()(lua_State* L) override {
        return (*this)(tuple_types<typename fx_traits::return_type>(), typename fx_traits::args_type(), L);
    }

    template<typename... Args>
    int operator()(types<void>, types<Args...> t, lua_State* L) {
        stack::pop_call(L, fx, t);
        return 0;
    }

    template<typename... TRn, typename... Args>
    int operator()(types<TRn...>, types<Args...> t, lua_State* L) {
        auto r = stack::pop_call(L, fx, t);
        stack::push(L, r);
        return sizeof...(TRn);
    }
};


int lua_cfun(lua_State* L) {
    void* bridgedata = lua_touserdata(L, lua_upvalueindex(1));
    auto* fx = static_cast<lua_func*>(bridgedata);
    int r = fx->operator()(L);
    return r;
}
} // detail
} // sol

#endif // SOL_LUA_FUNC_HPP
